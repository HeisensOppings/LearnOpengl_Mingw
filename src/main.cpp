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

    Shader::ShaderInit("E:/Project/OpenGL/src/shaders.glsl");
    Shader Program_cubes(0);
    Shader Program_light(1);
    Shader Program_buffe(3);
    // Shader Program_SkyBo(4);
    Shader Program_Depth(6);
    Shader Program_DQuad(7);
    // Shader Program_DCube(8);
    Shader Program_DCube(8, 8, 1);
    Shader Program_DSpot(9);

    Texture diffuseMapCubes("./src/image/Indoor_Dq_Build_Wood_01_T4_Diffuse.png", GL_REPEAT, GL_LINEAR);
    Texture diffuseMapPlane("./src/image/Indoor_Ly_Build_Floor_03_T4_Diffuse.png", GL_REPEAT, GL_LINEAR);

    Program_cubes.Bind();
    Program_cubes.SetUniform1i("material.diffuse", 0);
    // Program_cubes.SetUniform1i("material.specular", 1);
    Program_cubes.SetUniform1i("depthMapPoint", 1);
    Program_cubes.SetUniform1i("depthMapDir", 2);
    Program_cubes.SetUniform1i("depthMapSpot", 3);

    Program_buffe.Bind();
    Program_buffe.SetUniform1i("screenTexture", 5);

    Program_DQuad.Bind();
    Program_DQuad.SetUniform1i("depthMap", 0);

    Model ourModel_lisa("./model/lisa/lisa.obj");
    // Model ourModel_planet("./model/planet/planet.obj");
    // Model ourModel_rock("./model/rock/rock.obj");
    // Model ourModel2("./model/heita/heita.obj");
    Model ourModel_Syabugyo("./model/Dq_Syabugyo/IndoorScene_Dq_Syabugyo.obj");

    BufferLayout layout_Cubes;
    vector<int> layout_Cubes_Stride{3, 3, 2};
    layout_Cubes.AddFloat(layout_Cubes_Stride);
    VertexArray VAO_Cubes;
    VertexBuffer VBO_Cubes(CubesVertices, sizeof(CubesVertices));
    VAO_Cubes.AddBuffer(VBO_Cubes, layout_Cubes);

    BufferLayout layout_Plane;
    vector<int> layout_Plane_Stride{3, 3, 2};
    layout_Plane.AddFloat(layout_Plane_Stride);
    VertexArray VAO_Plane;
    VertexBuffer VBO_Plane(PlaneVertices, sizeof(PlaneVertices));
    VAO_Plane.AddBuffer(VBO_Plane, layout_Plane);

    BufferLayout layout_Frame;
    vector<int> layout_Frame_Stride{2, 2};
    layout_Frame.AddFloat(layout_Frame_Stride);
    VertexArray VAO_Frame;
    VertexBuffer VBO_Frame(FrameVertices, sizeof(FrameVertices));
    VAO_Frame.AddBuffer(VBO_Frame, layout_Frame);

    BufferLayout layout_SkyBoxs;
    layout_SkyBoxs.AddFloat(3);
    VertexArray VAO_SkyBoxs;
    VertexBuffer VBO_SkyBoxs(SkyboxVertices, sizeof(SkyboxVertices));
    VAO_SkyBoxs.AddBuffer(VBO_SkyBoxs, layout_SkyBoxs);

    // framebuffer configuraion
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a multismapled color attachment texture
    unsigned int textureColorBufferMultiSampled;
    glGenTextures(1, &textureColorBufferMultiSampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
    // create a color attachment texture
    // unsigned int textureColorbuffer;
    // glGenTextures(1, &textureColorbuffer);
    // glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // use a single renderbuffer object for both a depth AND stencil buffer
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    // now actually attach it
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // configure second post-processing framebuffer
    unsigned int intermediateFBO;
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
    // create a color attachment texture
    unsigned int screenTexture;
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ----------------------------------configure directional light depth map FBO
    unsigned int depthMapFBO_Direcational;
    glGenFramebuffers(1, &depthMapFBO_Direcational);
    // create depth texture
    unsigned int depthMap_Direcational;
    glGenTextures(1, &depthMap_Direcational);
    glBindTexture(GL_TEXTURE_2D, depthMap_Direcational);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH * 4, SCR_HEIGHT * 4, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_Direcational);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_Direcational, 0);
    glDrawBuffer(GL_NONE);  
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ----------------------------------configure point light depth map FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO_Point;
    glGenFramebuffers(1, &depthMapFBO_Point);
    // create depth cubemap texture
    unsigned int depthCubemap_Point;
    glGenTextures(1, &depthCubemap_Point);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap_Point);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_Point);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap_Point, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ----------------------------------configure spot light depth map FBO
    unsigned int depthMapFBO_Spot;
    glGenFramebuffers(1, &depthMapFBO_Spot);
    // create depth texture
    unsigned int depthMap_Spot;
    glGenTextures(1, &depthMap_Spot);
    glBindTexture(GL_TEXTURE_2D, depthMap_Spot);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH * 4, SCR_HEIGHT * 4, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_Spot);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_Spot, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);

#if 1 // -----------------------------------imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Lighting Settings");
        ImGui::PushItemWidth(150);
        // ImGui::SliderFloat("reflect rate", &refractive_rate, 1.00, 3.00);
        if (lighting_mode_camera)
            lightSpotPos = camera.m_cameraPos;
        ImGui::Checkbox("Camera spot light", &lighting_mode_camera);
        ImGui::Checkbox("Gamma", &gamma);
        if (ImGui::Button(("point distance: " + std::to_string(light_distance_index[light_distance_select_point])).c_str()))
        {
            ++light_distance_select_point;
            if (light_distance_select_point > 4)
                light_distance_select_point = 0;
        }
        if (ImGui::Button(("spot distance: " + std::to_string(light_distance_index[light_distance_select_spot])).c_str()))
        {
            ++light_distance_select_spot;
            if (light_distance_select_spot > 4)
                light_distance_select_spot = 0;
        }
        ImGui::ColorEdit3(" background_color", (float *)&background_color);
        ImGui::NewLine();
        ImGui::Text("ambient diffuse specular");
        ImGui::ColorEdit3(" directional", (float *)&sunlight_color);
        ImGui::SliderFloat3("directional", (float *)&light_am_di_sp_directional, 0.0f, 1.0f);
        ImGui::ColorEdit3(" point", (float *)&lightColor_point);
        ImGui::SliderFloat3("point", (float *)&light_am_di_sp_point, 0.0f, 1.0f);
        ImGui::ColorEdit3(" spot", (float *)&lightColor_spot);
        ImGui::SliderFloat3("spot", (float *)&light_am_di_sp_spot, 0.0f, 1.0f);
        ImGui::NewLine();
        if (ImGui::Button(("material shininess: " + std::to_string(material_shininess)).c_str()))
        {
            material_shininess += 32;
            if (material_shininess > 256)
                material_shininess = 32;
        }
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
#endif

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_Direcational);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SCR_WIDTH * 4, SCR_HEIGHT * 4);

#if 1 // --------------------------------------depthMap rendering for directional
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane_dir = 1.0f, far_plane_dir = 10.0f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane_dir, far_plane_dir);
        lightView = glm::lookAt(sunlight_pos / glm::vec3(50), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        Program_Depth.Bind();
        Program_Depth.SetUniform4m("lightSpaceMatrix", lightSpaceMatrix);
        {

            glm::mat4 model = glm::mat4(1.0f);
            VAO_Cubes.Bind();
            diffuseMapCubes.Bind();

            for (unsigned int i = 0; i < cubePositions.size(); i++)
            {
                Program_Depth.Bind();
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                float angle = (i + 1) * 0.5;
                model = glm::rotate(model, angle, glm::vec3(1.0f, 0.5f, 0.0f));
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
                Program_Depth.SetUniform4m("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.0f, -0.5f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            Program_Depth.SetUniform4m("model", model);
            ourModel_lisa.Draw(Program_Depth);

            VAO_Plane.Bind();
            diffuseMapPlane.Bind();
            model = glm::mat4(1.0f);
            Program_Depth.SetUniform4m("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
#endif

        lightPos.z = static_cast<float>(sin(glfwGetTime() * 0.5) * 3.0);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_Point);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

#if 1 // ------------------------------------- depthMap rendering for point light

        float near_plane = 0.1f;
        float far_plane = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
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
        diffuseMapCubes.Bind();

        for (unsigned int i = 0; i < cubePositions.size(); i++)
        // for (unsigned int i = 0; i < 1; i++)
        {
            Program_DCube.Bind();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = (i + 1) * 0.5;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.5f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            Program_DCube.SetUniform4m("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, -0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        Program_DCube.SetUniform4m("model", model);
        ourModel_lisa.Draw(Program_Depth);

        // VAO_Plane.Bind();
        // diffuseMapPlane.Bind();
        // model = glm::mat4(1.0f);
        // Program_DCube.SetUniform4m("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
#endif

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_Spot);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SCR_WIDTH * 4, SCR_HEIGHT * 4);

#if 1 // --------------------------------------depthMap rendering for spot
        glm::mat4 view_Spot;
        // view = camera.GetViewMatrix();
        view_Spot = Camera::calculate_lookAt_matrix(lightSpotPos, lightDirection, glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 projection_Spot = glm::mat4(1.0f);
        projection_Spot = glm::perspective(glm::radians(camera.m_Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, zbuffer_near, zbuffer_far);
        glm::mat4 lightSpaceMatrix_Spot = projection_Spot * view_Spot;
        {

            VAO_Cubes.Bind();
            diffuseMapCubes.Bind();
            Program_DSpot.Bind();
            Program_DSpot.SetUniform4m("view", view_Spot);
            Program_DSpot.SetUniform4m("projection", projection_Spot);
            for (unsigned int i = 0; i < cubePositions.size(); i++)
            {
                Program_DSpot.Bind();
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                float angle = (i + 1) * 0.5;
                model = glm::rotate(model, angle, glm::vec3(1.0f, 0.5f, 0.0f));
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
                Program_DSpot.SetUniform4m("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, -0.70f, 0.0f));
                model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
                Program_DSpot.SetUniform4m("model", model);
                // ourModel_Syabugyo.Draw(Program_cubes);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-1.0f, -0.5f, 0.0f));
                model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
                Program_DSpot.SetUniform4m("model", model);
                ourModel_lisa.Draw(Program_DSpot);
            }
        }
#endif

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

#if 1 // ------------------------------------- normal rendering
        {
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
#if 1 // --------------------------------------------light cubes rendering
            VAO_Cubes.Bind();
            Program_light.Bind();
            // sopt light
            Program_light.SetUniform4m("view", view);
            Program_light.SetUniform4m("projection", projection);
            Program_light.SetUniform3f("lightColor", lightColor_spot);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2f));
            Program_light.SetUniform4m("model", model);
            if (!lighting_mode_camera)
                glDrawArrays(GL_TRIANGLES, 0, 36);
            else
            {
                lightDirection = camera.m_cameraDir;
                lightSpotPos = camera.m_cameraPos;
            }

            // directional light
            Program_light.SetUniform3f("lightColor", sunlight_color);
            model = glm::mat4(1.0f);
            model = glm::translate(model, sunlight_pos);
            model = glm::scale(model, glm::vec3(50.0f)); // Make it a smaller cube
            Program_light.SetUniform4m("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);

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
            diffuseMapCubes.Bind();
            SceneLightConfig(Program_cubes, view, projection);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap_Point);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, depthMap_Direcational);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, depthMap_Spot);
            Program_cubes.SetUniform1f("far_plane", far_plane);

            Program_cubes.SetUniform4m("lightSpaceMatrix", lightSpaceMatrix);
            Program_cubes.SetUniform4m("lightSpaceMatrix_Spot", lightSpaceMatrix_Spot);

            for (unsigned int i = 0; i < cubePositions.size(); i++)
            // for (unsigned int i = 0; i < 1; i++)
            {
                Program_cubes.Bind();
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                float angle = (i + 1) * 0.5;
                model = glm::rotate(model, angle, glm::vec3(1.0f, 0.5f, 0.0f));
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
                glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
                Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
                Program_cubes.SetUniform3f("viewPos", camera.m_cameraPos);
                Program_cubes.SetUniform4m("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // // ------------------------------------------ plane
            // VAO_Plane.Bind();
            // diffuseMapPlane.Bind();
            // // Program_cubes.SetUniform1i("gamma", (int)gamma);
            // Program_cubes.SetUniform1f("scales", 2.0f);
            // model = glm::mat4(1.0f);
            // model = glm::translate(model, glm::vec3(0.0f, 0.0, 0.0));
            // glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            // Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
            // Program_cubes.SetUniform3f("viewPos", camera.m_cameraPos);
            // Program_cubes.SetUniform4m("model", model);
            // glDrawArrays(GL_TRIANGLES, 0, 6);

            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, -0.70f, 0.0f));
                model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
                Program_cubes.SetUniform4m("model", model);
                glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
                Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
                ourModel_Syabugyo.Draw(Program_cubes);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-1.0f, -0.5f, 0.0f));
                model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
                Program_cubes.SetUniform4m("model", model);
                normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
                Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
                ourModel_lisa.Draw(Program_cubes);
            }
        }
#endif

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        VAO_Frame.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        Program_buffe.Bind();
        glActiveTexture(GL_TEXTURE0 + 5);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        Program_buffe.SetUniform1i("mode", 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glViewport(0, 0, SCR_WIDTH / 4, SCR_HEIGHT / 4);
        Program_DQuad.Bind();
        Program_DQuad.SetUniform1f("near_plane", near_plane);
        Program_DQuad.SetUniform1f("far_plane", far_plane);
        // glViewport(0, 0, SCR_WIDTH / 4, SCR_HEIGHT / 4);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap_Direcational);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glViewport(0, SCR_HEIGHT / 4, SCR_WIDTH / 4, SCR_HEIGHT / 4);
        glBindTexture(GL_TEXTURE_2D, depthMap_Spot);
        glDrawArrays(GL_TRIANGLES, 0, 6);

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

    glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_FRAMEBUFFER_SRGB);

    // glEnable(GL_CULL_FACE);

    return 0;
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] int width, [[maybe_unused]] int height)
{
    // make sure the viewport matches the new window dimensions; note that width and height
    // will be significantly larger than specified on retina displays
    // glViewport(0, 0, width, height);
    // SCR_WIDTH = width;
    // SCR_HEIGHT = height;
    // int x = (width - SCR_WIDTH) / 4;
    // int y = (height - SCR_HEIGHT) / 4;
    // cout << width << " -/2 " << SCR_WIDTH << "=" << x << " " << height << " -/2 " << SCR_HEIGHT << "=" << y << endl;
    // glViewport(x, y, SCR_WIDTH, SCR_HEIGHT);
}

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
        controlKey = (action == GLFW_PRESS);
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
    static float speed_move = 5.0f;
    (void)window;
    (void)xoffset;
    if (!rightClick && controlKey)
        camera.ProcessMouseScroll(yoffset);
    else if (yoffset > 0)
    {
        speed_move += (speed_move < 1.0f) ? 0.1f : 1.0f;
        camera.SetSpeed(speed_move);
    }
    else if (yoffset < 0)
    {
        speed_move -= (speed_move <= 1.0f) ? 0.1f : 1.0f;
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
    shader.SetUniform1f("scales", 1.0f);
    // direction light
    // shader.SetUniform1i("depth_test", (int)depth_test);
    // shader.SetUniform1f("zbuffer_near", zbuffer_near);
    // shader.SetUniform1f("zbuffer_far", zbuffer_far);
    // shader.SetUniform1f("refractive_rate", refractive_rate);
    shader.SetUniform3f("dirLight.direction", sunlight_pos);
    shader.SetUniform3f("dirLight.ambient", sunlight_color * light_am_di_sp_directional.x);
    shader.SetUniform3f("dirLight.diffuse", sunlight_color * light_am_di_sp_directional.y);
    shader.SetUniform3f("dirLight.specular", sunlight_color * light_am_di_sp_directional.z);
    // point light
    for (GLuint i = 0; i < 1; i++)
    {
        string number = to_string(i);
        shader.SetUniform3f("pointLights[" + number + "].position", lightPos);
        // shader.SetUniform3f("pointLights[" + number + "].position", pointLightPositions[i]);
        shader.SetUniform3f("pointLights[" + number + "].ambient", lightColor_point * light_am_di_sp_point.x);
        shader.SetUniform3f("pointLights[" + number + "].diffuse", lightColor_point * light_am_di_sp_point.y);
        shader.SetUniform3f("pointLights[" + number + "].specular", lightColor_point * light_am_di_sp_point.z);
        shader.SetUniform1f("pointLights[" + number + "].constant", 1.0f);
        shader.SetUniform1f("pointLights[" + number + "].linear", light_distance[light_distance_select_point][0]);
        shader.SetUniform1f("pointLights[" + number + "].quadratic", light_distance[light_distance_select_point][1]);
    }
    // spotLight
    shader.SetUniform3f("spotLight.position", lightSpotPos);
    shader.SetUniform3f("spotLight.direction", lightDirection);
    shader.SetUniform3f("spotLight.ambient", lightColor_spot * light_am_di_sp_spot.x);
    shader.SetUniform3f("spotLight.diffuse", lightColor_spot * light_am_di_sp_spot.y);
    shader.SetUniform3f("spotLight.specular", lightColor_spot * light_am_di_sp_spot.z);
    shader.SetUniform1f("spotLight.constant", 1.0f);
    shader.SetUniform1f("spotLight.linear", light_distance[light_distance_select_spot][0]);
    shader.SetUniform1f("spotLight.quadratic", light_distance[light_distance_select_spot][1]);
    shader.SetUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader.SetUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
    shader.SetUniform1i("material.shininess", material_shininess);
}

void renderObject([[maybe_unused]] Shader &shader)
{
}
