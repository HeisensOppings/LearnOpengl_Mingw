#include "SceneCascadedShadowMap.h"

SceneCascadedShadowMap::SceneCascadedShadowMap(Camera *camera, unsigned int width, unsigned int height) : Scene(camera, width, height)
{
    // compile shader for model
    // ResourceManager::SetShader("modelAnim", 25, 24).Bind();
    // ResourceManager::GetShader("modelAnim").SetUniform4m("pvm", App::vp * mat4(1.0));
    ResourceManager::SetShader("model", 20, 24).Bind();
    ResourceManager::GetShader("model").SetUniform4m("pvm", App::vp * mat4(1.0));

    // compile shader for CSM
    ResourceManager::SetShader("shader", 26, 31).Bind();
    ResourceManager::SetShader("simpleDepthShader", 8, 32, 2).Bind();
    ResourceManager::SetShader("debugDepthQuad", 7, 33).Bind();
    ResourceManager::SetShader("debugCascadeShader", 27, 34).Bind();

    cameraViewMatric = camera->GetViewMatrix();
    modelHutao = new Model(string(RES_DIR) + "model/hutao/hutao_multi2.fbx");

    texture = new Texture(string(RES_DIR) + "model/Dq_Uyutei/Area_Dq_Build_Common_TsgWood_01_T4_Diffuse.png");

    // configure light FBO
    // -----------------------
    glGenFramebuffers(1, &lightFBO);
    glGenTextures(1, &lightDepthMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthMapResolution, depthMapResolution, int(shadowCascadeLevels.size()) + 1,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    constexpr float bordercolor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);
    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightDepthMaps, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
        throw 0;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // configure UBO
    // --------------------
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
    // --------- shader --- binding = 0
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // shader configuration
    // --------------------
    auto &shader = ResourceManager::GetShader("shader");
    shader.Bind();
    shader.SetUniform1i("diffuseTexture", 0);
    shader.SetUniform1i("shadowMap", 1);
    auto &debugDepthQuad = ResourceManager::GetShader("debugDepthQuad");
    debugDepthQuad.Bind();
    debugDepthQuad.SetUniform1i("depthMap", 0);
}

SceneCascadedShadowMap::~SceneCascadedShadowMap()
{
    DELETE_PTR(modelHutao)
    DELETE_PTR(texture);
    DELETE_PTR(planeVA);
    DELETE_PTR(quadVA);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glDeleteBuffers(1, &matricesUBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &lightFBO);
    glDeleteTextures(1, &lightDepthMaps);
}

void SceneCascadedShadowMap::Render()
{
    glEnable(GL_BLEND);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 0. UBO setup
    const auto lightMatrices = getLightSpaceMatrices();
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    for (size_t i = 0; i < lightMatrices.size(); ++i)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // 1. render depth of scene to texture (from light's perspective)
    // --------------------------------------------------------------
    // lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    // render scene from light's point of view
    auto &simpleDepthShader = ResourceManager::GetShader("simpleDepthShader");
    simpleDepthShader.Bind();
    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glViewport(0, 0, depthMapResolution, depthMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    // --------------------------
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT); // peter panning

    // glDisable(GL_CULL_FACE);

    renderScene(simpleDepthShader);
    // glEnable(GL_CULL_FACE);

    // glCullFace(GL_BACK);
    // glDisable(GL_CULL_FACE);
    //--------------------------

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // reset viewport
    glViewport(0, 0, App::scr_width, App::scr_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 2. render scene as normal using the generated depth/shadow map
    // --------------------------------------------------------------
    glViewport(0, 0, App::scr_width, App::scr_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto &shader = ResourceManager::GetShader("shader");
    shader.Bind();
    // glm::mat4 projection = glm::perspective(glm::radians(sceneCamera->m_Fov), (float)App::scr_width / (float)App::scr_height, cameraNearPlane, cameraFarPlane);
    glm::mat4 projection = glm::perspective(glm::radians(sceneCamera->m_Fov), (float)App::scr_width / (float)App::scr_height, cameraNearPlane, 50.0f);
    glm::mat4 view = sceneCamera->GetViewMatrix();
    shader.SetUniform4m("projection", AppControl::projection);
    shader.SetUniform4m("view", view);
    // set light uniforms
    shader.SetUniform3f("viewPos", sceneCamera->m_cameraPos);
    shader.SetUniform3f("lightDir", lightDir);
    // shader.SetUniform1f("farPlane", cameraFarPlane);
    shader.SetUniform1i("cascadeCount", shadowCascadeLevels.size());
    shader.SetUniform1f("bias_offs", AppControl::bias_offs);
    shader.SetUniform1f("bias_mids", AppControl::bias_mids);

    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
    {
        shader.SetUniform1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);

    renderScene(shader);

    // debug
    // --------------------------------------------------------------
    auto &debugCascadeShader = ResourceManager::GetShader("debugCascadeShader");
    if (lightMatricesCache.size() != 0)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        debugCascadeShader.Bind();
        debugCascadeShader.SetUniform4m("projection", projection);
        debugCascadeShader.SetUniform4m("view", view);
        drawVisualizerFrustums(&debugCascadeShader);
        drawCascadeVolumeVisualizers(lightMatricesCache, &debugCascadeShader);
        glDisable(GL_BLEND);
    }

    // render Depth map to quad for visual debugging
    // ---------------------------------------------
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
    auto &debugDepthQuad = ResourceManager::GetShader("debugDepthQuad");
    for (unsigned int Layer = 0; Layer <= shadowCascadeLevels.size(); ++Layer)
    {
        glViewport(0, (App::scr_height / 5) * Layer, App::scr_width / 5, App::scr_height / 5);
        debugDepthQuad.Bind();
        debugDepthQuad.SetUniform1i("layer", Layer);
        renderQuad();
    }

    glViewport(0, 0, App::scr_width, App::scr_height);
}

void SceneCascadedShadowMap::renderScene(Shader &shader)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
    shader.SetUniform4m("model", model);
    modelHutao->Draw(shader);

    if (planeVA == nullptr)
    {
        float planeVertices[] = {
            // positions            // normals         // texcoords
            -25.0f, 0.0f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // 左上
            25.0f, 0.0f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,   // 右上
            -25.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f, // 左下
            25.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f, // 右下
        };
        planeVA = new VertexArray({3, 3, 2}, planeVertices, sizeof(planeVertices));
    }
    // floor
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
    model = glm::mat4(1.0f);
    shader.SetUniform4m("model", model);
    planeVA->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SceneCascadedShadowMap::renderQuad()
{
    if (quadVA == nullptr)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // 右上
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // 左上
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // 右下
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // 左下
        };
        quadVA = new VertexArray({3, 2}, quadVertices, sizeof(quadVertices));
    }
    quadVA->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void SceneCascadedShadowMap::drawCascadeVolumeVisualizers(const std::vector<glm::mat4> &lightMatrices, Shader *shader)
{
    std::vector<GLuint> visualizerVAOs(8);
    std::vector<GLuint> visualizerVBOs(8);
    std::vector<GLuint> visualizerEBOs(8);
    const GLuint indices[] = {
        0, 2, 3,
        0, 3, 1,
        4, 6, 2,
        4, 2, 0,
        5, 7, 6,
        5, 6, 4,
        1, 3, 7,
        1, 7, 5,
        6, 7, 3,
        6, 3, 2,
        1, 5, 4,
        0, 1, 4};
    const glm::vec4 colors[] = {
        {1.0, 0.0, 0.0, 0.5f},
        {0.0, 1.0, 0.0, 0.5f},
        {0.0, 0.0, 1.0, 0.5f},
    };
    for (unsigned int i = 0; i < lightMatrices.size(); ++i)
    {
        if (shadowCascadeLevels.size() + 1 != debugLayer)
            if (i != debugLayer)
                continue;
        const auto corners = getFrustumCornersWorldSpace(lightMatrices[i]);
        // const auto corners = getFrustumCornersWorldSpace(lightMatrices[i]);
        std::vector<glm::vec3> vec3s;
        for (const auto &v : corners)
        {
            vec3s.push_back(glm::vec3(v));
        }
        glGenVertexArrays(1, &visualizerVAOs[i]);
        glGenBuffers(1, &visualizerVBOs[i]);
        glGenBuffers(1, &visualizerEBOs[i]);
        glBindVertexArray(visualizerVAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, visualizerVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizerEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glBindVertexArray(visualizerVAOs[i]);
        shader->SetUniform4f("color", colors[i % 3]);

        glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);
        glDeleteBuffers(1, &visualizerVBOs[i]);
        glDeleteBuffers(1, &visualizerEBOs[i]);
        glDeleteVertexArrays(1, &visualizerVAOs[i]);
        glBindVertexArray(0);
    }
}

// draw Cascade Frustums Visualizers
void SceneCascadedShadowMap::drawVisualizerFrustums(Shader *shader)
{
    unsigned int cascadeLevels = shadowCascadeLevels.size() + 1;
    std::vector<GLuint> visualizerFrustumVAOs(cascadeLevels);
    std::vector<GLuint> visualizerFrustumVBOs(cascadeLevels);
    std::vector<GLuint> visualizerFrustumEBOs(cascadeLevels);

    const GLuint indices[] = {
        0, 2, 3,
        0, 3, 1,
        4, 6, 2,
        4, 2, 0,
        5, 7, 6,
        5, 6, 4,
        1, 3, 7,
        1, 7, 5,
        6, 7, 3,
        6, 3, 2,
        1, 5, 4,
        0, 1, 4};
    const glm::vec4 colors[] = {
        {1.0, 1.0, 1.0, 0.5f},
        {0.8, 1.0, 1.0, 0.5f},
        {1.0, 0.2, 0.6, 0.5f},
        {0.4, 0.5, 0.8, 0.5f},
        {0.2, 1.0, 0.2, 0.5f},
    };
    for (unsigned int i = 0; i < cascadeLevels; ++i)
    {
        if (shadowCascadeLevels.size() + 1 != debugLayer)
            if (i != debugLayer)
                continue;
        float nearPlane = 0.0f;
        float farPlane = 0.0f;
        if (i == 0)
        {
            nearPlane = cameraNearPlane;
            farPlane = shadowCascadeLevels[i];
        }
        else if (i < shadowCascadeLevels.size())
        {
            nearPlane = shadowCascadeLevels[i - 1];
            farPlane = shadowCascadeLevels[i];
        }
        else
        {
            nearPlane = shadowCascadeLevels[i - 1];
            farPlane = cameraFarPlane;
        }
        const auto proj = glm::perspective(
            glm::radians(sceneCamera->m_Fov), (float)App::scr_width / (float)App::scr_height, nearPlane,
            farPlane);
        const auto corners = getFrustumCornersWorldSpace(proj, cameraViewMatric);

        //---------------------------------
        glm::vec3 center = glm::vec3(0, 0, 0);
        for (const auto &v : corners)
        {
            center += glm::vec3(v);
        }
        center /= corners.size();
        //---------------------------------
        std::vector<glm::vec3> vec3s;
        for (const auto &v : corners)
        {
            // const auto trf = lightView * v;
            // vec3s.push_back(glm::vec3(trf));
            vec3s.push_back(glm::vec3(v));
        }

        glGenVertexArrays(1, &visualizerFrustumVAOs[i]);
        glGenBuffers(1, &visualizerFrustumVBOs[i]);
        glGenBuffers(1, &visualizerFrustumEBOs[i]);

        glBindVertexArray(visualizerFrustumVAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, visualizerFrustumVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizerFrustumEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

        glBindVertexArray(visualizerFrustumVAOs[i]);
        shader->SetUniform4f("color", colors[i % 5]);
        glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);

        glDeleteBuffers(1, &visualizerFrustumVBOs[i]);
        glDeleteBuffers(1, &visualizerFrustumEBOs[i]);
        glDeleteVertexArrays(1, &visualizerFrustumVAOs[i]);

        glBindVertexArray(0);
    }
}

std::vector<glm::vec4> SceneCascadedShadowMap::getFrustumCornersWorldSpace(const glm::mat4 &projview)
{
    const auto inv = glm::inverse(projview);
    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }
    return frustumCorners;
}

std::vector<glm::vec4> SceneCascadedShadowMap::getFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view)
{
    return getFrustumCornersWorldSpace(proj * view);
}

glm::mat4 SceneCascadedShadowMap::getLightSpaceMatrix(const float nearPlane, const float farPlane)
{
    const auto proj = glm::perspective(
        glm::radians(45.0f), (float)App::scr_width / (float)App::scr_height, nearPlane,
        farPlane);
    // glm::mat4 cameraView = glm::lookAt(camera->m_cameraPos + lightDir, camera->m_cameraPos, glm::vec3(0.0f, 1.0f, 0.0f));
    // const auto corners = getFrustumCornersWorldSpace(proj, cameraView);
    const auto corners = getFrustumCornersWorldSpace(proj, sceneCamera->GetViewMatrix());
    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto &v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();
    const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto &v : corners)
    {
        const auto trf = lightView * v;

        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }
    // The direction facing in lightViewSpace () is -Z
    // So farPlane should be minZ and nearPlane should be maxZ
    // lookAt in center, that is, minZ < 0, maxZ > 0
    // The direction facing in lightProjectionSpace (NDC) is +Z
    // So farPlane = -minZ, nearPlane = -maxZ
    auto temp = -minZ;
    minZ = -maxZ;
    maxZ = temp;
    // Tune this parameter according to the scene
    // auto mid = (maxZ - minZ) / 2;
    // minZ -= mid * 5.0f;
    // maxZ += mid * 5.0f;
    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}

std::vector<glm::mat4> SceneCascadedShadowMap::getLightSpaceMatrices()
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(cameraNearPlane, shadowCascadeLevels[i]));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], cameraFarPlane));
        }
    }
    return ret;
}

void SceneCascadedShadowMap::processInput(GLFWwindow *window)
{
    static int plusPress = GLFW_RELEASE;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE && plusPress == GLFW_PRESS)
    {
        debugLayer++;
        if (debugLayer > shadowCascadeLevels.size() + 1)
        {
            debugLayer = 0;
        }
    }
    plusPress = glfwGetKey(window, GLFW_KEY_N);
    static int cPress = GLFW_RELEASE;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && cPress == GLFW_PRESS)
    {
        lightMatricesCache = getLightSpaceMatrices();
        cameraViewMatric = sceneCamera->GetViewMatrix();
    }
    cPress = glfwGetKey(window, GLFW_KEY_C);
}
