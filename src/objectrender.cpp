#include "objectrender.h"

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

void PBRObject::PBRInit(Shader &s)
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

    Shader &shaderIrradiance = ResourceManager::SetShader("Irradiance", 16, 18);
    Shader &shaderPrefilter = ResourceManager::SetShader("Prefilter", 16, 19);
    Shader &shaderBRDF = ResourceManager::SetShader("BRDF", 17, 20);

    VertexArray &VAO_Cubes = ResourceManager::GetVAO("cubes");
    VertexArray &VAO_Frame = ResourceManager::GetVAO("frame");

#if 1 // -----------------------PBR IBL
      // pbr: setup framebuffer
      // ----------------------
    unsigned int captureFBO, captureRBO;
    glf()->glGenFramebuffers(1, &captureFBO);
    glf()->glGenRenderbuffers(1, &captureRBO);

    glf()->glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glf()->glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glf()->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glf()->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: environment cubemap
    // The original HDR environment (newport_loft.hdr) and skybox were removed
    // on purpose: the scene now uses a solid background colour. To keep the
    // PBR image-based-lighting pipeline (irradiance / prefilter / BRDF LUT)
    // working, the environment cubemap is filled with a constant, neutral
    // colour so the models still receive a soft uniform ambient term.
    // ---------------------------------------------------------
    // unsigned int envCubemap;
    const int envSize = 32;
    std::vector<float> envData(envSize * envSize * 3);
    for (size_t p = 0; p < envData.size(); p += 3)
    {
        envData[p + 0] = AppControl::environment_color.r;
        envData[p + 1] = AppControl::environment_color.g;
        envData[p + 2] = AppControl::environment_color.b;
    }
    glf()->glGenTextures(1, &envCubemap);
    glf()->glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glf()->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, envSize, envSize, 0, GL_RGB, GL_FLOAT, envData.data());
    }
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glf()->glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glf()->glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    // unsigned int irradianceMap;
    glf()->glGenTextures(1, &irradianceMap);
    glf()->glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glf()->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // the irradiance map has no mip chain, so it must not use a mipmap min filter
    // (otherwise the texture is incomplete and samples as black).
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glf()->glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glf()->glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glf()->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    VAO_Cubes.Bind();
    shaderIrradiance.Bind();
    shaderIrradiance.SetUniform1i("environmentMap", 0);
    shaderIrradiance.SetUniform4m("projection", captureProjection);
    glf()->glActiveTexture(GL_TEXTURE0);
    glf()->glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glf()->glViewport(0, 0, 32, 32);
    glf()->glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        shaderIrradiance.SetUniform4m("view", captureViews[i]);
        glf()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glf()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glf()->glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glf()->glBindFramebuffer(GL_FRAMEBUFFER, AppControl::defaultFBO);

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    // unsigned int prefilterMap;
    glf()->glGenTextures(1, &prefilterMap);
    glf()->glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glf()->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16, 128, 128, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // be sure to set minification filter to mip_linear
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glf()->glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    shaderPrefilter.Bind();
    shaderPrefilter.SetUniform1i("environmentMap", 0);
    shaderPrefilter.SetUniform4m("projection", captureProjection);
    glf()->glActiveTexture(GL_TEXTURE0);
    glf()->glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glf()->glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // resize framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * pow(0.5, mip));
        glf()->glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glf()->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glf()->glViewport(0, 0, mipWidth, mipHeight);
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        shaderPrefilter.SetUniform1f("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            shaderPrefilter.SetUniform4m("view", captureViews[i]);
            glf()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glf()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glf()->glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glf()->glBindFramebuffer(GL_FRAMEBUFFER, AppControl::defaultFBO);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    // unsigned int brdfLUTTexture;
    glf()->glGenTextures(1, &brdfLUTTexture);
    // pre-allocate enough memory for the LUT texture;
    glf()->glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glf()->glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glf()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glf()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glf()->glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glf()->glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glf()->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glf()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
    glf()->glViewport(0, 0, 512, 512);
    VAO_Frame.Bind();
    shaderBRDF.Bind();
    glf()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glf()->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glf()->glBindFramebuffer(GL_FRAMEBUFFER, AppControl::defaultFBO);

    // initialize static shader uniforms before rendering
    glf()->glViewport(0, 0, AppControl::scr_width, AppControl::scr_height);
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
    glf()->glActiveTexture(GL_TEXTURE6);
    glf()->glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glf()->glActiveTexture(GL_TEXTURE7);
    glf()->glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glf()->glActiveTexture(GL_TEXTURE8);
    glf()->glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    // m_shader.SetUniform4m("view", AppControl::view);
    m_shader.SetUniform4m("vp", AppControl::vp);
    m_shader.SetUniform3f("camPos", camera_pos);
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
        glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(AppControl::elapsedTime * 5.0) * 5.0, 0.0, 0.0);
        // lightPositions[i] = newPos;
        newPos = lightPositions[i];
        m_shader.SetUniform3f("lightPositions[" + std::to_string(i) + "]", newPos);
        m_shader.SetUniform3f("lightColors[" + std::to_string(i) + "]", AppControl::irradiance_color * 400.0f);
    }
    m_shader.SetUniform4m("model", model);
    m_shader.SetUniform3m("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    m_model->Draw(m_shader);
}
