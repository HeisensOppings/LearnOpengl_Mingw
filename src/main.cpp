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

    float vertices_cube[] = {
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

    Shader::ShaderInit("E:/Project/OpenGL/src/shaders.glsl");
    Shader Program_cubes(0);
    Shader Program_light(1);

    Texture diffuseMap("E:/Project/OpenGL/src/image/container.png", GL_REPEAT, GL_LINEAR, 0);
    Texture specularMap("E:/Project/OpenGL/src/image/container_specular.png", GL_REPEAT, GL_LINEAR, 1);

    Program_cubes.Bind();
    Program_cubes.SetUniform1i("material.diffuse", 0);
    Program_cubes.SetUniform1i("material.specular", 1);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.5f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
        glm::vec3(1.0f, -1.5f, 1.0f)};

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    BufferLayout layout2;
    vector<int> layout_stride2{3, 3, 2};
    layout2.AddFloat(layout_stride2);
    VertexArray VAO_Cubes;
    VertexBuffer VBO2(vertices_cube, sizeof(vertices_cube));
    VAO_Cubes.AddBuffer(VBO2, layout2);

    glm::vec3 lightPos(0.0, 0.0, 1.0);
    glm::vec3 lightDirection(0.0f, 0.0f, -1.0f);

    glm::vec3 background_color(0.1);

    glm::vec3 sunlight_color(1.0f);
    glm::vec3 sunlight_pos(10.0f, 100.0f, 0.0f);

    // glm::vec3 lightColor_directional(1.0f);
    glm::vec3 light_am_di_sp_directional(0.1f, 0.5f, 1.0f);
    glm::vec3 lightColor_point(1.0f);
    glm::vec3 light_am_di_sp_point(0.1f, 0.6f, 1.0f);
    glm::vec3 lightColor_spot(1.0f);
    glm::vec3 light_am_di_sp_spot(0.1f, 0.8f, 1.0f);

    unsigned int light_distance_select_point = 2;
    unsigned int light_distance_select_spot = 4;
    vector<vector<float>> light_distance{{0.14, 0.07}, {0.07, 0.017}, {0.027, 0.0028}, {0.014, 0.0007}, {0.007, 0.0002}};
    vector<int> light_distance_index{32, 65, 160, 325, 600};
    float angle_limit = 89.0;
    float light_cutoff = 12.5;
    float light_outerCutOff = 15.5;

    int material_shininess = 32;

    bool lighting_mode_camera = false;
    vector<string> lighting_mode_text{"Directional Light", "Point lights ----", "Spotlight -------"};

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Lighting Settings");
            ImGui::PushItemWidth(150);
            if (lighting_mode_camera)
                lightPos = camera.m_cameraPos;
            if (ImGui::Button("Camera Light"))
                lighting_mode_camera = !lighting_mode_camera;
            if (ImGui::Button(("light point distance: " + std::to_string(light_distance_index[light_distance_select_point])).c_str()))
            {
                ++light_distance_select_point;
                if (light_distance_select_point > 4)
                    light_distance_select_point = 0;
            }
            if (ImGui::Button(("light spot distance: " + std::to_string(light_distance_index[light_distance_select_spot])).c_str()))
            {
                ++light_distance_select_spot;
                if (light_distance_select_spot > 4)
                    light_distance_select_spot = 0;
            }
            ImGui::ColorEdit3(" background_color", (float *)&background_color);
            ImGui::NewLine();
            ImGui::ColorEdit3(" color1 directional", (float *)&sunlight_color);
            ImGui::SliderFloat3("ambient-diffuse-specular1", (float *)&light_am_di_sp_directional, 0.0f, 1.0f);
            ImGui::ColorEdit3(" color2 point", (float *)&lightColor_point);
            ImGui::SliderFloat3("ambient-diffuse-specular2", (float *)&light_am_di_sp_point, 0.0f, 1.0f);
            ImGui::ColorEdit3(" color3 spot", (float *)&lightColor_spot);
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.m_Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        VAO_Cubes.Bind();
        Program_light.Bind();
        diffuseMap.Bind();

        // light cube
        Program_light.SetUniform4m("view", view);
        Program_light.SetUniform4m("projection", projection);
        Program_light.SetUniform3f("lightColor", lightColor_spot);
        glm::mat4 model = glm::mat4(1.0f);
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
        Program_light.SetUniform3f("lightColor", sunlight_color);
        model = glm::mat4(1.0f);
        model = glm::translate(model, sunlight_pos);
        model = glm::scale(model, glm::vec3(50.0f)); // Make it a smaller cube
        Program_light.SetUniform4m("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        Program_light.SetUniform3f("lightColor", lightColor_point);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            Program_light.SetUniform4m("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // small subes
        Program_cubes.Bind();
        // direction light
        Program_cubes.SetUniform3f("dirLight.direction", sunlight_pos);
        Program_cubes.SetUniform3f("dirLight.ambient", sunlight_color * light_am_di_sp_directional.x);
        Program_cubes.SetUniform3f("dirLight.diffuse", sunlight_color * light_am_di_sp_directional.y);
        Program_cubes.SetUniform3f("dirLight.specular", sunlight_color * light_am_di_sp_directional.z);
        // point light
        for (GLuint i = 0; i < 4; i++)
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

        Program_cubes.SetUniform4m("view", view);
        Program_cubes.SetUniform4m("projection", projection);
        model = glm::mat4(1.0f);
        Program_cubes.SetUniform4m("model", model);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
        Program_cubes.SetUniform3f("viewPos", camera.m_cameraPos);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = (i + 1) * glfwGetTime();
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.5f, 0.0f));
            glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
            Program_cubes.SetUniform3f("viewPos", camera.m_cameraPos);
            Program_cubes.SetUniform4m("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // big cube
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0, 0.0, -100.0));
        model = glm::scale(model, glm::vec3(100.0));
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
        Program_cubes.SetUniform3f("viewPos", camera.m_cameraPos);
        Program_cubes.SetUniform4m("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
