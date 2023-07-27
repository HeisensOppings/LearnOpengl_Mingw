#include "main.h"

#define SCR_WIDTH_DEFAULT 800
#define SCR_HEIGHT_DEFAULT 500
unsigned int SCR_WIDTH = SCR_WIDTH_DEFAULT;
unsigned int SCR_HEIGHT = SCR_HEIGHT_DEFAULT;

float key_value = 1.0;
int switchTexture = 1;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool rightClick = true;
bool firstMouse = true;

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

    float CubesVertices[] = {
        // positions          // normals        // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    float PlaneVertices[] = {
        // positions       // normals   // texture Coords
        5.0f, -0.5f, 5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
        5.0f, -0.5f, 5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
        5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 2.0f};

    float GrassVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
        1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f, 0.0f};

    Shader::ShaderInit("E:/Project/OpenGL/src/shaders.glsl");
    Shader Program_cubes(0);
    Shader Program_plane(0);
    Shader Program_light(1);
    Shader Program_grass(2);
    Shader Program_windo(2);

    Texture diffuseMapCubes("./src/image/container.png", GL_REPEAT, GL_LINEAR, 0);
    Texture specularMapCubes("./src/image/container_specular.png", GL_REPEAT, GL_LINEAR, 1);
    Texture diffuseMapPlane("./src/image/Indoor_Ly_Build_Floor_03_T4_Diffuse.png", GL_REPEAT, GL_LINEAR, 2);
    Texture specularMapPlane("./src/image/Indoor_Ly_Build_Floor_03_T4_SMBE.png", GL_REPEAT, GL_LINEAR, 3);
    Texture diffuseMapGrass("./src/image/grass_inazuma1.png", GL_CLAMP_TO_EDGE, GL_LINEAR, 4);
    Texture diffuseMapWindo("./src/image/blending_transparent_window.png", GL_CLAMP_TO_EDGE, GL_LINEAR, 5);

    Program_cubes.Bind();
    Program_cubes.SetUniform1i("material.diffuse", 0);
    Program_cubes.SetUniform1i("material.specular", 1);

    Program_plane.Bind();
    Program_plane.SetUniform1i("material.diffuse", 2);
    Program_plane.SetUniform1i("material.specular", 3);

    Program_grass.Bind();
    Program_grass.SetUniform1i("texture", 4);

    Program_windo.Bind();
    Program_windo.SetUniform1i("texture", 5);

    // Model ourModel1("./model/lisa/lisa.obj");
    // Model ourModel2("./model/heita/heita.obj");
    // Model ourModel3("./model/Dq_Syabugyo/IndoorScene_Dq_Syabugyo.obj");

    vector<glm::vec3>
        pointLightPositions{
            glm::vec3(0.0f, 4.0f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f, 2.0f, -12.0f),
            glm::vec3(0.0f, 0.0f, -3.0f)};

    vector<glm::vec3> cubePositions{
        // glm::vec3(0.0f, 0.0f, -15.0f),
        glm::vec3(-1.0f, 0.0f, -1.0f),
        // glm::vec3(-3.8f, 0.0f, -12.3f),
        glm::vec3(-1.0f, 0.0f, -1.0f),
        glm::vec3(-1.7f, 0.0f, -7.5f),
        glm::vec3(1.3f, 0.0f, -2.5f),
        // glm::vec3(1.5f, 0.0f, -2.5f),
        // glm::vec3(1.5f, 0.0f, -1.5f),
        glm::vec3(-1.3f, 0.0f, -1.5f),
        glm::vec3(1.0f, 0.0f, 1.0f)};

    vector<glm::vec3> vegetation{
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3(1.5f, 0.0f, 0.51f),
        glm::vec3(0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3(0.5f, 0.0f, -0.6f)};

    vector<glm::vec3> windows{
        glm::vec3(-1.5f, 0.0f, -0.4f),
        glm::vec3(1.5f, 0.0f, 0.45f),
        glm::vec3(0.0f, 0.0f, 0.2f),
        glm::vec3(-0.3f, 0.0f, -1.8f),
        glm::vec3(0.5f, 0.0f, -0.2f)};

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

    BufferLayout layout_Grass;
    vector<int> layout_Grass_Stride{3, 2};
    layout_Grass.AddFloat(layout_Grass_Stride);
    VertexArray VAO_Grass;
    VertexBuffer VBO_Grass(GrassVertices, sizeof(GrassVertices));
    VAO_Grass.AddBuffer(VBO_Grass, layout_Grass);

    glm::vec3 background_color(0.1);

    float zbuffer_near = 0.1;
    float zbuffer_far = 100;
    glm::vec3 lightPos(0.0, 0.0, 1.0);
    glm::vec3 lightDirection(0.0f, 0.0f, -1.0f);

    glm::vec3 sunlight_color(1.0f);
    glm::vec3 sunlight_pos(10.0f, 100.0f, 100.0f);

    glm::vec3 light_am_di_sp_directional(0.3f, 0.6f, 0.2f);
    glm::vec3 lightColor_point(1.0f);
    glm::vec3 light_am_di_sp_point(0.1f, 0.5f, 0.1f);
    glm::vec3 lightColor_spot(1.0f);
    glm::vec3 light_am_di_sp_spot(0.1f, 0.8f, 0.1f);

    unsigned int light_distance_select_point = 2;
    unsigned int light_distance_select_spot = 2;
    vector<vector<float>> light_distance{{0.14, 0.07}, {0.07, 0.017}, {0.027, 0.0028}, {0.014, 0.0007}, {0.007, 0.0002}};
    vector<int> light_distance_index{32, 65, 160, 325, 600};

    int material_shininess = 32;

    bool lighting_mode_camera = false;
    bool depth_test = false;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Lighting Settings");
            ImGui::PushItemWidth(150);
            if (ImGui::Button("z depth_test"))
                depth_test = !depth_test;
            if (depth_test)
            {
                ImGui::SliderFloat("zbuffer_near", &zbuffer_near, 0.01, 100.0);
                ImGui::SliderFloat("zbuffer_far", &zbuffer_far, zbuffer_near, 200.0);
            }
            ImGui::NewLine();
            if (lighting_mode_camera)
                lightPos = camera.m_cameraPos;
            if (ImGui::Button("Camera Light"))
                lighting_mode_camera = !lighting_mode_camera;
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
            ImGui::ColorEdit3(" directional", (float *)&sunlight_color);
            ImGui::SliderFloat3("ambient-diffuse-specular1", (float *)&light_am_di_sp_directional, 0.0f, 1.0f);
            ImGui::ColorEdit3(" point", (float *)&lightColor_point);
            ImGui::SliderFloat3("ambient-diffuse-specular2", (float *)&light_am_di_sp_point, 0.0f, 1.0f);
            ImGui::ColorEdit3(" spot", (float *)&lightColor_spot);
            ImGui::SliderFloat3("ambient-diffuse-specular3", (float *)&light_am_di_sp_spot, 0.0f, 1.0f);
            ImGui::NewLine();
            if (ImGui::Button(("material shininess: " + std::to_string(material_shininess)).c_str()))
            {
                material_shininess += 32;
                if (material_shininess > 256)
                    material_shininess = 32;
            }
            ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view;
        view = camera.GetViewMatrix();
        float aspect = 1.0f;
        if (SCR_HEIGHT != 0 || SCR_WIDTH != 0)
        {
            aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        }
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.m_Fov), aspect, zbuffer_near, zbuffer_far);
        glm::mat4 model = glm::mat4(1.0f);

        // -------------------------------------------- light cubes
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
            lightPos = camera.m_cameraPos;
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
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            Program_light.SetUniform4m("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // -------------------------------------- cubes
        VAO_Cubes.Bind();
        diffuseMapCubes.Bind();
        specularMapCubes.Bind();
        Program_cubes.Bind();
        Program_cubes.SetUniform4m("view", view);
        Program_cubes.SetUniform4m("projection", projection);
        // direction light
        Program_cubes.SetUniform1i("depth_test", (int)depth_test);
        Program_cubes.SetUniform1f("zbuffer_near", zbuffer_near);
        Program_cubes.SetUniform1f("zbuffer_far", zbuffer_far);
        Program_cubes.SetUniform3f("dirLight.direction", sunlight_pos);
        Program_cubes.SetUniform3f("dirLight.ambient", sunlight_color * light_am_di_sp_directional.x);
        Program_cubes.SetUniform3f("dirLight.diffuse", sunlight_color * light_am_di_sp_directional.y);
        Program_cubes.SetUniform3f("dirLight.specular", sunlight_color * light_am_di_sp_directional.z);
        // point light
        for (GLuint i = 0; i < 1; i++)
        {
            string number = to_string(i);
            Program_cubes.SetUniform3f("pointLights[" + number + "].position", pointLightPositions[i]);
            Program_cubes.SetUniform3f("pointLights[" + number + "].ambient", lightColor_point * light_am_di_sp_point.x);
            Program_cubes.SetUniform3f("pointLights[" + number + "].diffuse", lightColor_point * light_am_di_sp_point.y);
            Program_cubes.SetUniform3f("pointLights[" + number + "].specular", lightColor_point * light_am_di_sp_point.z);
            Program_cubes.SetUniform1f("pointLights[" + number + "].constant", 1.0f);
            Program_cubes.SetUniform1f("pointLights[" + number + "].linear", light_distance[light_distance_select_point][0]);
            Program_cubes.SetUniform1f("pointLights[" + number + "].quadratic", light_distance[light_distance_select_point][1]);
        }
        // spotLight
        Program_cubes.SetUniform3f("spotLight.position", lightPos);
        Program_cubes.SetUniform3f("spotLight.direction", lightDirection);
        Program_cubes.SetUniform3f("spotLight.ambient", lightColor_spot * light_am_di_sp_spot.x);
        Program_cubes.SetUniform3f("spotLight.diffuse", lightColor_spot * light_am_di_sp_spot.y);
        Program_cubes.SetUniform3f("spotLight.specular", lightColor_spot * light_am_di_sp_spot.z);
        Program_cubes.SetUniform1f("spotLight.constant", 1.0f);
        Program_cubes.SetUniform1f("spotLight.linear", light_distance[light_distance_select_spot][0]);
        Program_cubes.SetUniform1f("spotLight.quadratic", light_distance[light_distance_select_spot][1]);
        Program_cubes.SetUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        Program_cubes.SetUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        Program_cubes.SetUniform1i("material.shininess", material_shininess);

        // for (unsigned int i = 0; i < cubePositions.size(); i++)
        // {
        //     Program_cubes.Bind();
        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, cubePositions[i]);
        //     // float angle = (i + 1) * 0.5;
        //     // model = glm::rotate(model, angle, glm::vec3(1.0f, 0.5f, 0.0f));
        //     glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        //     Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
        //     Program_cubes.SetUniform3f("viewPos", camera.m_cameraPos);
        //     Program_cubes.SetUniform4m("model", model);
        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        // ---------------------------------------------- plane
        VAO_Plane.Bind();
        diffuseMapPlane.Bind();
        specularMapPlane.Bind();
        Program_plane.Bind();
        Program_plane.SetUniform4m("view", view);
        Program_plane.SetUniform4m("projection", projection);

        // direction light
        Program_plane.SetUniform1i("depth_test", (int)depth_test);
        Program_plane.SetUniform1f("zbuffer_near", zbuffer_near);
        Program_plane.SetUniform1f("zbuffer_far", zbuffer_far);
        Program_plane.SetUniform3f("dirLight.direction", sunlight_pos);
        Program_plane.SetUniform3f("dirLight.ambient", sunlight_color * light_am_di_sp_directional.x);
        Program_plane.SetUniform3f("dirLight.diffuse", sunlight_color * light_am_di_sp_directional.y);
        Program_plane.SetUniform3f("dirLight.specular", sunlight_color * light_am_di_sp_directional.z);
        // point light
        for (GLuint i = 0; i < 1; i++)
        {
            string number = to_string(i);
            Program_plane.SetUniform3f("pointLights[" + number + "].position", pointLightPositions[i]);
            Program_plane.SetUniform3f("pointLights[" + number + "].ambient", lightColor_point * light_am_di_sp_point.x);
            Program_plane.SetUniform3f("pointLights[" + number + "].diffuse", lightColor_point * light_am_di_sp_point.y);
            Program_plane.SetUniform3f("pointLights[" + number + "].specular", lightColor_point * light_am_di_sp_point.z);
            Program_plane.SetUniform1f("pointLights[" + number + "].constant", 1.0f);
            Program_plane.SetUniform1f("pointLights[" + number + "].linear", light_distance[light_distance_select_point][0]);
            Program_plane.SetUniform1f("pointLights[" + number + "].quadratic", light_distance[light_distance_select_point][1]);
        }
        // spotLight
        Program_plane.SetUniform3f("spotLight.position", lightPos);
        Program_plane.SetUniform3f("spotLight.direction", lightDirection);
        Program_plane.SetUniform3f("spotLight.ambient", lightColor_spot * light_am_di_sp_spot.x);
        Program_plane.SetUniform3f("spotLight.diffuse", lightColor_spot * light_am_di_sp_spot.y);
        Program_plane.SetUniform3f("spotLight.specular", lightColor_spot * light_am_di_sp_spot.z);
        Program_plane.SetUniform1f("spotLight.constant", 1.0f);
        Program_plane.SetUniform1f("spotLight.linear", light_distance[light_distance_select_spot][0]);
        Program_plane.SetUniform1f("spotLight.quadratic", light_distance[light_distance_select_spot][1]);
        Program_plane.SetUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        Program_plane.SetUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        Program_plane.SetUniform1i("material.shininess", material_shininess);

        model = glm::mat4(1.0f);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        Program_plane.SetUniform3m("normalMatrix", normalMatrix);
        Program_plane.SetUniform3f("viewPos", camera.m_cameraPos);
        Program_plane.SetUniform4m("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ---------------------------------------------- grass
        VAO_Grass.Bind();
        diffuseMapGrass.Bind();
        Program_grass.Bind();
        Program_grass.SetUniform4m("view", view);
        Program_grass.SetUniform4m("projection", projection);
        for (unsigned int i = 0; i < vegetation.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, vegetation[i]);
            // glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            // Program_grass.SetUniform3m("normalMatrix", normalMatrix);
            // Program_grass.SetUniform3f("viewPos", camera.m_cameraPos);
            Program_grass.SetUniform4m("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // windows

        // distance update
        map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); ++i)
        {
            float distance = glm::length(camera.m_cameraPos - windows[i]);
            sorted[distance] = windows[i];
        }

        diffuseMapWindo.Bind();
        Program_windo.Bind();
        Program_windo.SetUniform4m("view", view);
        Program_windo.SetUniform4m("projection", projection);
        for (map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            Program_windo.SetUniform4m("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // for (unsigned int i = 0; i < windows.size(); i++)
        // {
        //     model = glm::mat4(1.0f);
        //     model = glm::translate(model, windows[i]);
        //     // glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        //     // Program_grass.SetUniform3m("normalMatrix", normalMatrix);
        //     // Program_grass.SetUniform3f("viewPos", camera.m_cameraPos);
        //     Program_windo.SetUniform4m("model", model);
        //     glDrawArrays(GL_TRIANGLES, 0, 6);
        // }

        // // render the loaded model
        // // house
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, -0.22f, 0.0f));
        // model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
        // Program_cubes.SetUniform4m("model", model);
        // ourModel3.Draw(Program_cubes);
        // // lisa
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        // Program_cubes.SetUniform4m("model", model);
        // ourModel1.Draw(Program_cubes);
        // // heita
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(-1.0f, -0.12f, 0.0f));
        // model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        // Program_cubes.SetUniform4m("model", model);
        // ourModel2.Draw(Program_cubes);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    saveCameraPosition(camera);

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
    glfwSetWindowPos(window, 300, 540);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthFunc(GL_LESS);
    // glEnable(GL_STENCIL_TEST);
    // glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

    loadCameraPosition(camera);

    return 0;
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and height
    // will be significantly larger than specified on retina displays
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void processInput(GLFWwindow *window)
{
    // if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    // {
    //     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //     rightClick = true;
    //     firstMouse = true;
    // }
    // else
    // {
    //     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //     rightClick = false;
    // }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        key_value -= 1.0f * deltaTime;
        if (key_value <= 0.0f)
            key_value = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        key_value += 1.0f * deltaTime;
        if (key_value >= 1.0f)
            key_value = 1.0f;
    }
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
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        switchTexture = !switchTexture;
        // check powershell version : $PSVersionTable.PSVersion
        // if Major < 6, you need to install powershell version higher than 7.0.7
        // open 7.0.7 version int common ternimal : pwsh
        // set regedit value : Set-ItemProperty HKCU:\Console VirtualTerminalLevel -Type DWORD 1
        // then you ternimal powershell in vscode support color display
        cout << "\x1B[44mswitchTexture: \033[49m" << switchTexture << "\033[49m" << endl;
    }
    else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
    {
        static float speed = 20.0;
        speed += 1.0;
        if (speed >= 30)
            speed = 1.0;
        camera.SetSpeed(speed);
    }
    else if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
    }
    else if (key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
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
    (void)window;
    (void)xoffset;
    if (!rightClick)
        camera.ProcessMouseScroll(yoffset);
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

void loadCameraPosition(Camera &camera)
{
    std::ifstream file("camera.ini");
    if (file.is_open())
    {
        float posX, posY, posZ, dirX, dirY, dirZ, upX, upY, upZ, yaw, pitch, fov;
        file >> posX >> posY >> posZ;
        file >> dirX >> dirY >> dirZ;
        file >> upX >> upY >> upZ;
        file >> yaw;
        file >> pitch;
        file >> fov;
        file.close();
        camera.SetCameraSettings(posX, posY, posZ, dirX, dirY, dirZ, upX, upY, upZ, yaw, pitch, fov);
    }
}

void saveCameraPosition(const Camera &camera)
{
    std::ofstream file("camera.ini");
    if (file.is_open())
    {
        file << camera.m_cameraPos.x << " " << camera.m_cameraPos.y << " " << camera.m_cameraPos.z << "\n";
        file << camera.m_cameraDir.x << " " << camera.m_cameraDir.y << " " << camera.m_cameraDir.z << "\n";
        file << camera.m_cameraUp.x << " " << camera.m_cameraUp.y << " " << camera.m_cameraUp.z << "\n";
        file << camera.m_Yaw << "\n";
        file << camera.m_Pitch << "\n";
        file << camera.m_Fov << "\n";
        file.close();
    }
}
