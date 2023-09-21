#include "main.h"

#define SCR_WIDTH_DEFAULT 800
#define SCR_HEIGHT_DEFAULT 600

int SCR_WIDTH = SCR_WIDTH_DEFAULT;
int SCR_HEIGHT = SCR_HEIGHT_DEFAULT;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool rightClick = true;
bool firstMouse = true;
bool controlKey = false;

Camera camera;
GLFWwindow *window = nullptr;
void renderCube();

float ourLerp(float a, float b, float f)
{
    return a + f * (b - a);
}

int main()
{
    if (opengl_init() != 0)
    {
        cout << "Failed to initialize OpenGL" << endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    const char *glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Shader::ShaderInit("./src/shaders.glsl");
    Shader Program_cubes(0);
    // Shader Program_light(1);
    Shader Program_buffe(3);
    // Shader Program_Depth(6);
    // Shader Program_DCube(8, 8, 1);
    // Shader Program_DSpot(9);
    // Shader ShaderBlur(10);
    // Shader ShaderBloom(11);
    Shader shaderGeometryPass(12);
    Shader shaderLightingPass(13);
    Shader shaderLightBox(14);
    // Shader::CodeOutput = true;
    Shader shaderSSAO(15);
    // Shader::CodeOutput = false;
    Shader shaderSSAOBlur(15, 16);

    shaderGeometryPass.Bind();
    shaderGeometryPass.SetUniform1i("material.diffuse", 0);
    shaderGeometryPass.SetUniform1i("normalMap", 1);

    shaderLightingPass.Bind();
    shaderLightingPass.SetUniform1i("gPosition", 0);
    shaderLightingPass.SetUniform1i("gNormal", 1);
    shaderLightingPass.SetUniform1i("gAlbedo", 2);
    shaderLightingPass.SetUniform1i("gSSAO", 3);

    shaderSSAO.Bind();
    shaderSSAO.SetUniform1i("gPosition", 0);
    shaderSSAO.SetUniform1i("gNormal", 1);
    shaderSSAO.SetUniform1i("texNoise", 2);

    shaderSSAOBlur.Bind();
    shaderSSAOBlur.SetUniform1i("ssaoInput", 0);

    // Texture diffuseMapCubes("./src/image/Wall/bricks2.jpg");
    // Texture normalMapCubes("./src/image/Wall/bricks2_normal.jpg", GL_TEXTURE1);
    // Texture heightMapCubes("./src/image/Wall/bricks2_disp.jpg", GL_TEXTURE3);

    // Program_cubes.Bind();
    // Program_cubes.SetUniform1i("material.diffuse", 0);
    // Program_cubes.SetUniform1i("normalMap", 1);
    // // Program_cubes.SetUniform1i("heightMap", 3);
    // Program_cubes.SetUniform1i("depthMapDir", 5);
    // Program_cubes.SetUniform1i("depthMapPoint", 6);
    // Program_cubes.SetUniform1i("depthMapSpot", 7);

    Program_buffe.Bind();
    Program_buffe.SetUniform1i("screenTexture", 0);

    // ShaderBlur.Bind();
    // ShaderBlur.SetUniform1i("image", 0);

    // ShaderBloom.Bind();
    // ShaderBloom.SetUniform1i("scene", 0);
    // ShaderBloom.SetUniform1i("bloomBlur", 1);

    // Model Model_kafuka("./model/kafuka/kafuka.obj");
    Model Model_Aru("./model/Aru/Aru.obj");
    // Model Model_kafuka_knife("./model/kafuka/knife.obj");
    // Model Model_kafuka_knife("./model/kafuka/knife.obj");
    Model Model_raiden("./model/raiden/raiden.obj");
    // Model ourModel_Syabugyo("./model/Dq_Syabugyo/IndoorScene_Dq_Syabugyo.obj");
    Model ourModel_Syabugyo("./model/Dq_Uyutei/IndoorScene_Dq_Uyutei.obj");

    BufferLayout layout_Cubes;
    layout_Cubes.AddFloat(3, 3, 2);
    VertexArray VAO_Cubes;
    VertexBuffer VBO_Cubes(CubesVertices, sizeof(CubesVertices));
    VAO_Cubes.AddBuffer(VBO_Cubes, layout_Cubes);

    BufferLayout layout_Frame;
    layout_Frame.AddFloat(3, 2);
    VertexArray VAO_Frame;
    VertexBuffer VBO_Frame(FrameVertices, sizeof(FrameVertices));
    VAO_Frame.AddBuffer(VBO_Frame, layout_Frame);

    // configure g-buffer framebuffer
    // ------------------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedo;
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments_[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments_);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // also create framebuffer to hold SSAO processing stage
    // -----------------------------------------------------
    unsigned int ssaoFBO, ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // lighting info
    const unsigned int NR_LIGHTS = 32;
    vector<glm::vec3> lightPositions;
    vector<glm::vec3> lightColors;
    srand(13);
    for (unsigned int i = 0; i < NR_LIGHTS; ++i)
    {
        // calculate slightly random offsets
        float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
        // also calculate random color
        float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5);
        float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5);
        float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5);
        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }

    // generate sample kernel
    // ----------------------
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = ourLerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);
        // glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);

#if 1 // -----------------------------------imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Lighting Settings");
        ImGui::PushItemWidth(150);
        // ImGui::SliderFloat("reflect rate", &refractive_rate, 1.00, 3.00);
        if (lighting_mode_camera)
        {
            lightSpotPos = camera.m_cameraPos;
        }

        // ImGui::Checkbox("Camera spot light", &lighting_mode_camera);
        ImGui::Checkbox("SSAO", &hasSSAO);
        // ImGui::Checkbox("Gamma", &gamma);
        // if (ImGui::Button(("point distance: " + std::to_string(light_distance_index[light_distance_select_point])).c_str()))
        // {
        //     ++light_distance_select_point;
        //     if (light_distance_select_point > 4)
        //         light_distance_select_point = 0;
        // }
        // if (ImGui::Button(("spot distance: " + std::to_string(light_distance_index[light_distance_select_spot])).c_str()))
        // {
        //     ++light_distance_select_spot;
        //     if (light_distance_select_spot > 4)
        //         light_distance_select_spot = 0;
        // }
        ImGui::Text("Pos: %0.2f,%0.2f,%0.2f", camera.m_cameraPos.x, camera.m_cameraPos.y, camera.m_cameraPos.z);
        ImGui::Text("(%.3f ms)(%.1f fps)", 1000.0f / io.Framerate, io.Framerate);
        // ImGui::SliderFloat("exposure", (float *)&exposure, 0.001, 5.0);
        // ImGui::Checkbox("HDR", &hdr);
        // ImGui::ColorEdit3(" background_color", (float *)&background_color);
        // ImGui::NewLine();
        // ImGui::Text("ambient diffuse specular");
        // ImGui::ColorEdit3(" directional", (float *)&sunlight_color);
        // ImGui::SliderFloat3("directional", (float *)&light_am_di_sp_directional, 0.0f, 10.0f);
        // ImGui::ColorEdit3(" point", (float *)&lightColor_point);
        // ImGui::SliderFloat3("Color", (float *)&lightColor_point, 0.0f, 10.0f);
        // ImGui::SliderFloat3("point", (float *)&light_am_di_sp_point, 0.0f, 10.0f);
        // ImGui::ColorEdit3(" spot", (float *)&lightColor_spot);
        // ImGui::SliderFloat3("spot", (float *)&light_am_di_sp_spot, 0.0f, 10.0f);
        // ImGui::NewLine();
        // if (ImGui::Button(("material shininess: " + std::to_string(material_shininess)).c_str()))
        // {
        //     material_shininess += 32;
        //     if (material_shininess > 256)
        //         material_shininess = 32;
        // }
        ImGui::End();
#endif

        // framebuffer_direcatinal.Bind();

        // glm::mat4 lightProjection, lightView;
        // glm::mat4 lightSpaceMatrix;
        // float near_plane_dir = 1.0f, far_plane_dir = 10.0f;
        // lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane_dir, far_plane_dir);
        // lightView = glm::lookAt(sunlight_pos / glm::vec3(50), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        // lightSpaceMatrix = lightProjection * lightView;
#if 0 // --------------------------------------depthMap rendering for directional
      Program_Depth.Bind();
      Program_Depth.SetUniform4m("lightSpaceMatrix", lightSpaceMatrix);
      {

            glm::mat4 model = glm::mat4(1.0f);
            VAO_Cubes.Bind();

            Program_Depth.Bind();
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            Program_Depth.SetUniform4m("model", model);
            Model_kafuka.Draw(Program_Depth);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            Program_Depth.SetUniform4m("model", model);
            Model_raiden.Draw(Program_Depth);
        }
#endif

        // lightPos.z = static_cast<float>(sin(glfwGetTime() * 0.5) * 4.0);
        // framebuffer_point.Bind();

#if 0 // ------------------------------------- depthMap rendering for point light

        float near_plane = 0.1f;
        float far_plane = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        Program_DCube.Bind();
        for (unsigned int i = 0; i < 6; ++i)
            Program_DCube.SetUniform4m("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        Program_DCube.SetUniform1f("far_plane", far_plane);
        Program_DCube.SetUniform3f("lightPos", lightPos);

        VAO_Cubes.Bind();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        Program_DCube.SetUniform4m("model", model);
        Model_kafuka.Draw(Program_Depth);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        Program_DCube.SetUniform4m("model", model);
        Model_raiden.Draw(Program_Depth);
#endif

        // framebuffer_spot.Bind();

        // glm::mat4 view_Spot;
        // view_Spot = Camera::calculate_lookAt_matrix(lightSpotPos, lightDirection, glm::vec3(0.0, 1.0, 0.0));
        // glm::mat4 projection_Spot = glm::mat4(1.0f);
        // projection_Spot = glm::perspective(glm::radians(camera.m_Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, zbuffer_near, zbuffer_far);
        // glm::mat4 lightSpaceMatrixSpot = projection_Spot * view_Spot;
#if 0 // --------------------------------------depthMap rendering for spot
        {

            VAO_Cubes.Bind();
            Program_DSpot.Bind();
            Program_DSpot.SetUniform4m("view", view_Spot);
            Program_DSpot.SetUniform4m("projection", projection_Spot);
            {
                glm::mat4 model = glm::mat4(1.0f);
                // model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
                // model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
                // Program_DSpot.SetUniform4m("model", model);
                // ourModel_Syabugyo.Draw(Program_cubes);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
                Program_DSpot.SetUniform4m("model", model);
                Model_kafuka.Draw(Program_DSpot);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
                Program_DSpot.SetUniform4m("model", model);
                Model_raiden.Draw(Program_DSpot);
            }
        }
#endif

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 1 // ------------------------------------- normal rendering
      // {
        glm::mat4 view;
        view = camera.GetViewMatrix();
        float aspect = 1.0f;
        if (SCR_HEIGHT != 0 || SCR_WIDTH != 0)
        {
            aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        }
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.m_Fov), aspect, zbuffer_near, zbuffer_far);
        // projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -0.01f, 1000.0f);
        glm::mat4 model = glm::mat4(1.0f);
#if 0 // --------------------------------------------light cubes rendering
            VAO_Cubes.Bind();
            Program_light.Bind();
            // sopt light
            Program_light.SetUniform4m("view", view);
            Program_light.SetUniform4m("projection", projection);
            Program_light.SetUniform3f("lightColor", lightColor_spot);
            model = glm::translate(model, lightSpotPos);
            model = glm::scale(model, glm::vec3(0.1f));
            Program_light.SetUniform4m("model", model);
            if (!lighting_mode_camera)
                glDrawArrays(GL_TRIANGLES, 0, 36);
            else
            {
                lightDirection = camera.m_cameraDir;
                lightSpotPos = camera.m_cameraPos;
            }

            // directional light
            // Program_light.SetUniform3f("lightColor", sunlight_color);
            // model = glm::mat4(1.0f);
            // model = glm::translate(model, sunlight_pos);
            // model = glm::scale(model, glm::vec3(50.0f)); // Make it a smaller cube
            // Program_light.SetUniform4m("model", model);
            // glDrawArrays(GL_TRIANGLES, 0, 36);

            // point light
            Program_light.SetUniform3f("lightColor", lightColor_point);
            for (unsigned int i = 0; i < 1; i++)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, lightPos);
                // model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                Program_light.SetUniform4m("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
#endif
        VAO_Cubes.Bind();
        // diffuseMapCubes.Bind();
        // SceneLightConfig(Program_cubes, view, projection);
        // framebuffer_point.TextureBind();
        // framebuffer_direcatinal.TextureBind();
        // framebuffer_spot.TextureBind();

        // Program_cubes.SetUniform1f("far_plane", far_plane);

        // Program_cubes.SetUniform4m("lightSpaceMatrix", lightSpaceMatrix);
        // Program_cubes.SetUniform4m("lightSpaceMatrixSpot", lightSpaceMatrixSpot);
        shaderGeometryPass.Bind();
        shaderGeometryPass.SetUniform4m("projection", projection);
        shaderGeometryPass.SetUniform4m("view", view);
        shaderGeometryPass.SetUniform1i("hasNormalMap", 1);
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 4.0f));
            model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
            shaderGeometryPass.SetUniform4m("model", model);
            // glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
            shaderGeometryPass.SetUniform3m("normalMatrix", normalMatrix);
            ourModel_Syabugyo.Draw(shaderGeometryPass);

            shaderGeometryPass.SetUniform1i("hasNormalMap", 0);

            // model = glm::mat4(1.0f);
            // model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
            // model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            // shaderGeometryPass.SetUniform4m("model", model);
            // // normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            // normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
            // shaderGeometryPass.SetUniform3m("normalMatrix", normalMatrix);
            // Model_kafuka.Draw(shaderGeometryPass);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            shaderGeometryPass.SetUniform4m("model", model);
            // normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
            shaderGeometryPass.SetUniform3m("normalMatrix", normalMatrix);
            Model_raiden.Draw(shaderGeometryPass);

            // model = glm::mat4(1.0f);
            // model = glm::translate(model, glm::vec3(-0.1f, 0.8f, 0.0f));
            // model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            // shaderGeometryPass.SetUniform4m("model", model);
            // normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            // shaderGeometryPass.SetUniform3m("normalMatrix", normalMatrix);
            // Model_kafuka_knife.Draw(shaderGeometryPass);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(2.5f, 1.0f, 0.5f));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
            shaderGeometryPass.SetUniform4m("model", model);
            // normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
            shaderGeometryPass.SetUniform3m("normalMatrix", normalMatrix);
            Model_Aru.Draw(shaderGeometryPass);
        }
        // }

#endif
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        VAO_Frame.Bind();

        // 2. generate SSAO texture
        // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAO.Bind();
        // Send kernel + rotation
        for (unsigned int i = 0; i < 64; ++i)
            shaderSSAO.SetUniform3f("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        shaderSSAO.SetUniform4m("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. blur SSAO texture to remove noise
        // ------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAOBlur.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderLightingPass.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        shaderLightingPass.SetUniform1b("hasSSAO", hasSSAO);
        // send light relevant uniforms
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            // shaderLightingPass.SetUniform3f("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            shaderLightingPass.SetUniform3f("lights[" + std::to_string(i) + "].Position",glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPositions[i], 1.0)));
            shaderLightingPass.SetUniform3f("lights[" + std::to_string(i) + "].Color", lightColors[i]);
            // update attenuation parameters and calculate radius
            const float linear = 0.7f;
            const float quadratic = 1.8f;
            shaderLightingPass.SetUniform1f("lights[" + std::to_string(i) + "].Linear", linear);
            shaderLightingPass.SetUniform1f("lights[" + std::to_string(i) + "].Quadratic", quadratic);
        }
        // shaderLightingPass.SetUniform3f("viewPos", camera.m_cameraPos);
        // finally render quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //-------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. render lights on top of scene
        // --------------------------------
        VAO_Cubes.Bind();
        shaderLightBox.Bind();
        shaderLightBox.SetUniform4m("projection", projection);
        shaderLightBox.SetUniform4m("view", view);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.125f));
            shaderLightBox.SetUniform4m("model", model);
            shaderLightBox.SetUniform3f("lightColor", lightColors[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // ------------------------------------------------------
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        VAO_Frame.Bind();
        Program_buffe.Bind();
        glActiveTexture(GL_TEXTURE0);

        glViewport(0, 0, SCR_WIDTH / 4, SCR_HEIGHT / 4);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glViewport(0, SCR_HEIGHT / 4, SCR_WIDTH / 4, SCR_HEIGHT / 4);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glViewport(0, SCR_HEIGHT / 2, SCR_WIDTH / 4, SCR_HEIGHT / 4);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glViewport(0, SCR_HEIGHT / 4 * 3, SCR_WIDTH / 4, SCR_HEIGHT / 4);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    camera.saveCameraPosition(window);

    glfwTerminate();
    return 0;
}

int opengl_init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "mashiro", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    camera.loadCameraPosition(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetScrollCallback(window, scroll_callback);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_FRAMEBUFFER_SRGB);

    // glEnable(GL_CULL_FACE);

    return 0;
}

// void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] int width, [[maybe_unused]] int height)
// {
// make sure the viewport matches the new window dimensions; note that width and height
// will be significantly larger than specified on retina displays
// glViewport(0, 0, width, height);
// SCR_WIDTH = width;
// SCR_HEIGHT = height;
// int x = (width - SCR_WIDTH) / 4;
// int y = (height - SCR_HEIGHT) / 4;
// cout << width << " -/2 " << SCR_WIDTH << "=" << x << " " << height << " -/2 " << SCR_HEIGHT << "=" << y << endl;
// glViewport(x, y, SCR_WIDTH, SCR_HEIGHT);
// }

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyBoard(MOVE_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyBoard(MOVE_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyBoard(MOVE_FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyBoard(MOVE_BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.ProcessKeyBoard(MOVE_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.ProcessKeyBoard(MOVE_DOWN, deltaTime);
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)(window);
    (void)(scancode);
    (void)(mods);
    if (key == GLFW_KEY_LEFT_CONTROL)
    {
        controlKey = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    (void)window;
    static float lastX = xpos, lastY = ypos;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }
    if (!rightClick)
    {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    static float speed_move = 2.0f;
    (void)window;
    (void)xoffset;
    if (!rightClick && controlKey)
    {
        camera.ProcessMouseScroll(yoffset);
    }
    else if (yoffset > 0)
    {
        speed_move += (speed_move < 1.0f) ? 0.1f : 0.5f;
        // speed_move += 0.1;
        camera.SetSpeed(speed_move);
    }
    else if (yoffset < 0)
    {
        speed_move -= (speed_move <= 1.0f) ? 0.1f : 0.5f;
        // speed_move -= 0.1;
        speed_move = std::max(speed_move, 0.1f);
        camera.SetSpeed(speed_move);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        static bool flag = true;
        if (flag)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            rightClick = false;
            firstMouse = true;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            rightClick = true;
        }
        flag = !flag;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
    }
}

void SceneLightConfig(Shader &shader, glm::mat4 view, glm::mat4 projection)
{
    shader.Bind();
    shader.SetUniform4m("view", view);
    shader.SetUniform4m("projection", projection);
    // shader.SetUniform1f("scales", 1.0f);
    // direction light
    // shader.SetUniform1i("depth_test", (int)depth_test);
    // shader.SetUniform1f("zbuffer_near", zbuffer_near);
    // shader.SetUniform1f("zbuffer_far", zbuffer_far);
    // shader.SetUniform1f("refractive_rate", refractive_rate);

    // shader.SetUniform3f("dirLight.direction", sunlight_pos);
    // shader.SetUniform3f("dirLightDirecation", sunlight_pos);
    // shader.SetUniform3f("dirLight.ambient", sunlight_color * light_am_di_sp_directional.x);
    // shader.SetUniform3f("dirLight.diffuse", sunlight_color * light_am_di_sp_directional.y);
    // shader.SetUniform3f("dirLight.specular", sunlight_color * light_am_di_sp_directional.z);
    // point light
    shader.SetUniform3f("pointLightPos", lightPos);
    shader.SetUniform3f("viewPos", camera.m_cameraPos);
    shader.SetUniform3f("pointLight.position", lightPos);
    shader.SetUniform3f("pointLight.ambient", lightColor_point * light_am_di_sp_point.x);
    shader.SetUniform3f("pointLight.diffuse", lightColor_point * light_am_di_sp_point.y);
    shader.SetUniform3f("pointLight.specular", lightColor_point * light_am_di_sp_point.z);
    shader.SetUniform1f("pointLight.constant", 1.0f);
    shader.SetUniform1f("pointLight.linear", light_distance[light_distance_select_point][0]);
    shader.SetUniform1f("pointLight.quadratic", light_distance[light_distance_select_point][1]);
    // spotLight
    // shader.SetUniform3f("spotLight.position", lightSpotPos);
    // shader.SetUniform3f("spotLightPos", lightSpotPos);
    // shader.SetUniform3f("spotLight.direction", lightDirection);
    // shader.SetUniform3f("spotLightDirecation", lightDirection);
    // shader.SetUniform3f("spotLight.ambient", lightColor_spot * light_am_di_sp_spot.x);
    // shader.SetUniform3f("spotLight.diffuse", lightColor_spot * light_am_di_sp_spot.y);
    // shader.SetUniform3f("spotLight.specular", lightColor_spot * light_am_di_sp_spot.z);
    // shader.SetUniform1f("spotLight.constant", 1.0f);
    // shader.SetUniform1f("spotLight.linear", light_distance[light_distance_select_spot][0]);
    // shader.SetUniform1f("spotLight.quadratic", light_distance[light_distance_select_spot][1]);
    // shader.SetUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    // shader.SetUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
    // shader.SetUniform1i("material.shininess", material_shininess);
}

// FrameBuffer framebuffer_intermediate(SCR_WIDTH, SCR_HEIGHT, GL_TEXTURE_2D);
// FrameBuffer framebuffer_intermediate1(SCR_WIDTH, SCR_HEIGHT, GL_TEXTURE_2D);

// unsigned int hdrFBO;
// glGenFramebuffers(1, &hdrFBO);
// glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
// unsigned int colorBuffers[2];
// glGenTextures(2, colorBuffers);
// for (unsigned int i = 0; i < 2; i++)
// {
//     glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBuffers[i]);
//     glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, SCR_WIDTH * 4, SCR_HEIGHT * 4, GL_TRUE);
//     glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, colorBuffers[i], 0);
// }
// unsigned int rboDepth;
// glGenRenderbuffers(1, &rboDepth);
// glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH * 4, SCR_HEIGHT * 4);
// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
// unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
// glDrawBuffers(2, attachments);
// // finally check if framebuffer is complete
// if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//     std::cout << "Framebuffer not complete!" << std::endl;
// glBindFramebuffer(GL_FRAMEBUFFER, 0);

// unsigned int pingpongFBO[2];
// unsigned int pingpongColorbuffers[2];
// glGenFramebuffers(2, pingpongFBO);
// glGenTextures(2, pingpongColorbuffers);
// for (unsigned int i = 0; i < 2; i++)
// {
//     glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
//     glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
//     // also check if framebuffers are complete (no need for depth buffer)
//     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//         std::cout << "Framebuffer not complete!" << std::endl;
// }

// glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
// glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// /*rendering-------------------------------------------------------------------------------------*/
// glBindFramebuffer(GL_READ_FRAMEBUFFER, hdrFBO);
// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate.GetFrameBufferID());
// glReadBuffer(GL_COLOR_ATTACHMENT0);
// glDrawBuffer(GL_COLOR_ATTACHMENT0);
// glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

// glBindFramebuffer(GL_READ_FRAMEBUFFER, hdrFBO);
// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate1.GetFrameBufferID());
// glReadBuffer(GL_COLOR_ATTACHMENT1);
// glDrawBuffer(GL_COLOR_ATTACHMENT0);
// glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
