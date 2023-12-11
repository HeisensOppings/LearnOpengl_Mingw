#include "objectrender.h"

SKYObject::SKYObject(Shader &s, VertexArray &vao, unsigned int skymap)
    : m_shader(s), m_VAO(vao), m_skymap(skymap)
{
    if (skymap == 0)
        throw std::runtime_error("skymap is empty");
    m_shader.Bind();
    m_shader.SetUniform1i("skybox", 0);
    m_shader.SetUniform4m("projection", AppControl::projection);
}

void SKYObject::Render()
{
    m_VAO.Bind();
    m_shader.Bind();
    m_shader.SetUniform4m("view", AppControl::view);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skymap);
    // shaderSKY.SetUniform1f("_fresnel", _fresnel);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

const glm::vec3 lightPositions[] = {
    glm::vec3(-10.0f, 10.0f, 10.0f),
    glm::vec3(10.0f, 10.0f, 10.0f),
    glm::vec3(-10.0f, -10.0f, 10.0f),
    glm::vec3(10.0f, -10.0f, 10.0f),
};

bool PBRObject::initialized = false;
unsigned int PBRObject::envCubemap = 0;
unsigned int PBRObject::irradianceMap = 0;
unsigned int PBRObject::prefilterMap = 0;
unsigned int PBRObject::brdfLUTTexture = 0;

void PBRObject::PBRInit(Shader &s, string hdr_map)
{
    s.Bind();
    s.SetUniform1i("albedoMap", 0);
    s.SetUniform1i("normalMap", 1);
    s.SetUniform1i("metallicMap", 2);
    s.SetUniform1i("roughnessMap", 3);
    s.SetUniform1i("aoMap", 4);
    s.SetUniform1i("emissiveMap", 5);

    s.SetUniform1i("irradianceMap", 6);
    s.SetUniform1i("prefilterMap", 7);
    s.SetUniform1i("brdfLUT", 8);
    s.SetUniform1b("hasEmissive", 1);

    Shader &shaderHDR = ResourceManager::SetShader("HDR", 17, 19);
    Shader &shaderIrradiance = ResourceManager::SetShader("Irradiance", 17, 20);
    Shader &shaderPrefilter = ResourceManager::SetShader("Prefilter", 17, 21);
    Shader &shaderBRDF = ResourceManager::SetShader("BRDF", 18, 22);

    VertexArray &VAO_Cubes = ResourceManager::GetVAO("cubes");
    VertexArray &VAO_Frame = ResourceManager::GetVAO("frame");

#if 1 // -----------------------PBR IBL
      // pbr: setup framebuffer
      // ----------------------
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    // ---------------------------------
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(hdr_map.c_str(), &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    // unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    VAO_Cubes.Bind();
    shaderHDR.Bind();
    shaderHDR.SetUniform1i("equirectangularMap", 0);
    shaderHDR.SetUniform4m("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        shaderHDR.SetUniform4m("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ------------------ test
    // envCubemap = hdrTexture;

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    // unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    shaderIrradiance.Bind();
    shaderIrradiance.SetUniform1i("environmentMap", 0);
    shaderIrradiance.SetUniform4m("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        shaderIrradiance.SetUniform4m("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    // unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16, 128, 128, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // be sure to set minification filter to mip_linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    shaderPrefilter.Bind();
    shaderPrefilter.SetUniform1i("environmentMap", 0);
    shaderPrefilter.SetUniform4m("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // resize framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        shaderPrefilter.SetUniform1f("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            shaderPrefilter.SetUniform4m("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    // unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);
    // pre-allocate enough memory for the LUT texture;
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
    glViewport(0, 0, 512, 512);
    VAO_Frame.Bind();
    shaderBRDF.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // initialize static shader uniforms before rendering
    glViewport(0, 0, AppControl::scr_width, AppControl::scr_height);
#endif

    initialized = true;
}

PBRObject::PBRObject(Shader &s, string model_path) : m_shader(s)
{
    if (!initialized)
        throw std::runtime_error("PBRObject not init");
    m_model = new Model(model_path);
}

void PBRObject::Render(mat4 &model, vec3 camera_pos)
{
    m_shader.Bind();
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    // m_shader.SetUniform4m("view", AppControl::view);
    m_shader.SetUniform4m("vp", AppControl::vp);
    m_shader.SetUniform3f("camPos", camera_pos);
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
        glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
        // lightPositions[i] = newPos;
        newPos = lightPositions[i];
        m_shader.SetUniform3f("lightPositions[" + std::to_string(i) + "]", newPos);
        m_shader.SetUniform3f("lightColors[" + std::to_string(i) + "]", AppControl::irradiance_color * 400.0f);
    }
    m_shader.SetUniform4m("model", model);
    m_shader.SetUniform3m("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    m_model->Draw(m_shader);
}

void TextObject::LoadString(u32string u32str, vec2 screenPos, vec2 typography)
{
    m_u32str = u32str;
    m_Typography = typography;
    vector<float> vao_str;
    vector<GLuint> ibo_str;
    TextTexture::ChToTexture(m_u32str);

    unsigned int charsAdded = 0;
    for (auto c : m_u32str)
    {
        static float x_pos = screenPos.x;
        Character &ch = TextTexture::Characters[c];
        if (c == '\n')
        {
            screenPos.y -= (FONT_SIZE + m_Typography.x);
            screenPos.x = x_pos;
        }
        else
        {
            if (c != ' ')
            {
                float xpos = (screenPos.x + ch.Bearing.x);
                float ypos = (screenPos.y - (ch.Size.y - ch.Bearing.y));
                float w = ch.Size.x;
                float h = ch.Size.y;

                // update VBO for each character
                // float vertices[4][4] = {
                //     {xpos, ypos, 0.0f, 1.0f},
                //     {xpos, ypos + h, 0.0f, 0.0f},
                //     {xpos + w, ypos, 1.0f, 1.0f},
                //     {xpos + w, ypos + h, 1.0f, 0.0f}};
                // vec2(pos), vec4(texcoord(pre-calculate), texcoord )
                vector<float> vertices = {
                    xpos, ypos, ch.Offset.x, ch.Offset.y + ch.Offset.w, 0.0f, 1.0f,
                    xpos, ypos + h, ch.Offset.x, ch.Offset.y, 0.0f, 0.0f,
                    xpos + w, ypos, ch.Offset.x + ch.Offset.z, ch.Offset.y + ch.Offset.w, 1.0f, 1.0f,
                    xpos + w, ypos + h, ch.Offset.x + ch.Offset.z, ch.Offset.y, 1.0f, 0.0f};
                vao_str.insert(vao_str.end(), vertices.begin(), vertices.end());

                unsigned int startIndex = charsAdded * 4;
                ibo_str.push_back(startIndex);
                ibo_str.push_back(startIndex + 1);
                ibo_str.push_back(startIndex + 2);
                ibo_str.push_back(startIndex + 2);
                ibo_str.push_back(startIndex + 1);
                ibo_str.push_back(startIndex + 3);
                ++charsAdded;
            }
            screenPos.x += ((ch.Advance) + m_Typography.y);
        }
    }
    if (m_VAO != nullptr)
        delete m_VAO;
    m_VAO = new VertexArray({2, 4}, vao_str.data(), sizeof(float) * vao_str.size(), GL_STATIC_DRAW, ibo_str.data(), ibo_str.size(), GL_STATIC_DRAW);
}

void TextObject::Render(mat4 &model)
{
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    m_shader.Bind();
    // m_shader.SetUniform3f("texColor", AppControl::text_color);
    static float movement = 0.1;
    movement += 0.3 * AppControl::deltaTime;
    m_shader.SetUniform1f("deltaTime", (movement));
    m_shader.SetUniform4m("mvp", AppControl::vp * model);
    m_shader.SetUniform1f("thickness", AppControl::thickness);
    m_shader.SetUniform1f("softness", AppControl::softness);
    m_shader.SetUniform1f("outline_thickness", AppControl::outline_thickness);
    m_shader.SetUniform1f("outline_softness", AppControl::outline_softness);
    m_shader.SetUniform2f("shadow_offset", AppControl::text_shadow_offset / 1000.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextTexture::GetTetureID());

    (*m_VAO).Bind();
    glDrawElements(GL_TRIANGLES, (*m_VAO).GetIndexBuffer().GetCount(), GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}