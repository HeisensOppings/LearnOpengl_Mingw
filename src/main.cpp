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
    Texture emissionMap("E:/Project/OpenGL/src/image/matrix.png", GL_REPEAT, GL_LINEAR, 2);

    Program_cubes.Bind();
    Program_cubes.SetUniform1i("material.diffuse", 0);
    Program_cubes.SetUniform1i("material.specular", 1);
    Program_cubes.SetUniform1i("material.emission", 2);

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
        glm::vec3(1.0f, -1.5f, 1.0f),
    };

    BufferLayout layout2;
    vector<int> layout_stride2{3, 3, 2};
    layout2.AddFloat(layout_stride2);
    VertexArray VAO_Cubes;
    VertexBuffer VBO2(vertices_cube, sizeof(vertices_cube));
    VAO_Cubes.AddBuffer(VBO2, layout2);

    glm::vec3 lightPos(0.0, 0.0, 1.0);
    glm::vec3 lightDirection(0.0f, 0.0f, -1.0f);
    glm::vec3 lightColor(1.0f);
    glm::vec3 light_am_di_sp(0.2f, 0.8f, 1.0f);

    unsigned int light_distance_select = 4;
    vector<vector<float>> light_distance{{0.14, 0.07}, {0.07, 0.017}, {0.027, 0.0028}, {0.014, 0.0007}, {0.007, 0.0002}};
    vector<int> light_distance_index{32, 65, 160, 325, 600};
    float light_constant = 1.0f;
    float angle_limit = 89.0;
    float light_cutoff = 12.5;
    float light_outerCutOff = 15.5;
    int light_mode = 0; // directionalLight pointLight soptLight 

    int material_shininess = 32;
    bool material_emission = 0;

    bool emission_switch = false;
    bool lighting_mode_camera = false;
    vector<string> lighting_mode_text{"Directional Light", "Point lights ----", "Spotlight -------"};

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Lighting Settings");
            ImGui::PushItemWidth(150);
            if (lighting_mode_camera)
                lightPos = camera.m_cameraPos;
            if (ImGui::Button("Camera Light"))
                lighting_mode_camera = !lighting_mode_camera;
            if (ImGui::Button(lighting_mode_text[light_mode].c_str()))
            {
                ++light_mode;
                if (light_mode > 2)
                    light_mode = 0;
            }
            ImGui::SliderInt("mode", &light_mode, 0, 2);
            if (ImGui::Button(("light distance: " + std::to_string(light_distance_index[light_distance_select])).c_str()))
            {
                ++light_distance_select;
                if (light_distance_select > 4)
                    light_distance_select = 0;
            }
            ImGui::ColorEdit3("light color rgb", (float *)&lightColor);
            ImGui::SliderFloat3("light color vec", (float *)&lightColor, 0.0f, 1.0f);
            ImGui::SliderFloat3("ambient-diffuse-specular", (float *)&light_am_di_sp, 0.0f, 1.0f);
            ImGui::NewLine();
            if (ImGui::Button(("material shininess: " + std::to_string(material_shininess)).c_str()))
            {
                material_shininess += 32;
                if (material_shininess > 256)
                    material_shininess = 32;
            }
            ImGui::Checkbox("emission", &material_emission);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // float radius = 2.0f;
        // float camX = sin(glfwGetTime()) * radius;
        // float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.m_Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        VAO_Cubes.Bind();
        Program_light.Bind();
        diffuseMap.Bind();
        specularMap.Bind();
        emissionMap.Bind();

        // light cube
        Program_light.SetUniform4m("view", view);
        Program_light.SetUniform4m("projection", projection);
        Program_light.SetUniform3f("lightColor", lightColor);
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

        // small subes
        Program_cubes.Bind();
        Program_cubes.SetUniform3f("light.ambient", lightColor * light_am_di_sp.x);
        Program_cubes.SetUniform3f("light.diffuse", lightColor * light_am_di_sp.y);
        Program_cubes.SetUniform3f("light.specular", lightColor * light_am_di_sp.z);
        Program_cubes.SetUniform1i("material.shininess", material_shininess);
        Program_cubes.SetUniform1i("material.emission_mode", (int)material_emission);
        Program_cubes.SetUniform1i("light_mode", light_mode);

        // Program_cubes.SetUniform3f("cameraDir", light_dir_test);
        // Program_cubes.SetUniform3f("cameraDir", camera.m_cameraPos);

        // Program_cubes.SetUniform3f("light.position", lightPos);
        // Program_cubes.SetUniform3f("light.direction", -lightPos);
        Program_cubes.SetUniform3f("light.position", lightPos);
        Program_cubes.SetUniform3f("light.direction", lightDirection);
        // Program_cubes.SetUniform3f("light.direction", -camera.m_cameraPos);
        // Program_cubes.SetUniform3f("light.position", light_dir_test);
        // Program_cubes.SetUniform3f("light.direction", -light_dir_test);
        Program_cubes.SetUniform1f("light.constant", 1.0f);
        Program_cubes.SetUniform1f("light.linear", light_distance[light_distance_select][0]);
        Program_cubes.SetUniform1f("light.quadratic", light_distance[light_distance_select][1]);
        Program_cubes.SetUniform1f("light.cutoff", glm::cos(glm::radians(28.0f)));
        Program_cubes.SetUniform1f("light.outerCutOff", glm::cos(glm::radians(28.0f)));
        // Program_cubes.SetUniform3f("lightPos", light_dir_test);
        // Program_cubes.SetUniform3f("lightPos", glm::vec3(camX, 2.0, camZ));
        Program_cubes.SetUniform4m("view", view);
        Program_cubes.SetUniform4m("projection", projection);
        model = glm::mat4(1.0f);
        Program_cubes.SetUniform4m("model", model);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
        Program_cubes.SetUniform3f("viewPos", camera.m_cameraPos);
        Program_cubes.SetUniform1f("yoffset", glfwGetTime());
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
        cout << key_value << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        key_value += 1.0f * deltaTime;
        if (key_value >= 1.0f)
            key_value = 1.0f;
        cout << key_value << endl;
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
        cout << "mouse1 " << endl;
    }
    else if (key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        cout << "mouse2 " << endl;
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

// #shader vertex
// #version 330 core
// layout(location = 0) in vec3 aPos;
// layout(location = 1) in vec3 aNormal;
// layout(location = 2) in vec2 aTexCoords;
// out vec3 FragPos;
// out vec3 Normal;
// out vec2 TexCoords;
// uniform mat3 normalMatrix;
// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;
// void main()
// {
//     FragPos = vec3(model * vec4(aPos, 1.0));
//     Normal = normalize(normalMatrix * aNormal);
//     gl_Position = projection * view * vec4(FragPos, 1.0);
//     TexCoords = aTexCoords;
// }

// #shader fragment
// #version 330 core
// out vec4 FragColor;
// struct Material
// {
//     sampler2D diffuse;
//     int shininess;
// };
// struct Light
// {
//     vec3 position;
//     vec3 direction;
//     vec3 diffuse;
//     vec3 specular;
//     float constant;
//     float linear;
//     float quadratic;
//     float cutoff;
//     float outerCutOff;
// };
// in vec3 Normal;
// in vec3 FragPos;
// in vec2 TexCoords;
// uniform Material material;
// uniform Light light;
// void main()
// {
//     // diffuse
//     vec3 norm = normalize(Normal);
//     vec3 lightDir = normalize(light.position - FragPos);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
//     // spotlight
//     float theta = dot(lightDir, normalize(-light.direction));
//     float epsilon = (light.cutoff - light.outerCutOff);
//     float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//     diffuse *= intensity;
//     // result
//     FragColor = vec4((diffuse), 1.0);
// }