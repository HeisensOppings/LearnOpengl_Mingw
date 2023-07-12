#include "main.h"

#define SCR_WIDTH_DEFAULT 800
#define SCR_HEIGHT_DEFAULT 500
unsigned int SCR_WIDTH = SCR_WIDTH_DEFAULT;
unsigned int SCR_HEIGHT = SCR_HEIGHT_DEFAULT;

float key_value = 1.0;
int switchTexture = 1;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool altPressed = false;
bool firstMouse = true;

Camera camera;
GLFWwindow *window = nullptr;

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        static bool flag = false;
        if (flag)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            altPressed = false;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            altPressed = true;
            firstMouse = true;
        }
        flag = !flag;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
    }
}

int main()
{
    if (opengl_init() != 0)
    {
        cout << "Failed to initialize OpenGL" << endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    const char *glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    float vertices3[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    float vertices_multi_cubes[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    float vertices_cube[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};

    const string filepath = "E:/Project/OpenGL/src/shaders.glsl";
    Shader::ShaderInit(filepath);
    Shader Program1(0, 0);
    Shader Program_cubes(1, 1);
    Shader Program_light(2, 2);

    // float vertices1[] = {
    //     // positions      // texture coords (note that we changed them to 2.0f!)
    //     0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right
    //     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
    //     -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left
    // };

    // float vertices1[] = {
    //     // positions          // colors           // texture coords (note that we changed them to 2.0f!)
    //     0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
    //     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    //     -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    // };

    // float vertices2[] = {
    //     // positions      // texture coords (note that we changed them to 2.0f!)
    //     0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // top right
    //     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
    //     -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,  // top left
    // };
    // int indices[] = {1, 2, 2, 3, 3, 1};//GL_LINES
    // int indices[] = {0, 1, 3, 1, 2, 3};

    vector<int> layout_stride1{3, 2, 3};
    BufferLayout layout1;
    layout1.AddFloat(layout_stride1);

    VertexArray VAO1;
    VertexBuffer VBO1(vertices_multi_cubes, sizeof(vertices_multi_cubes));
    // IndexBuffer IBO1(indices, 6);
    VAO1.AddBuffer(VBO1, layout1);

    Texture texture1("E:/Project/OpenGL/src/image/texture1.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    Texture texture2("E:/Project/OpenGL/src/image/texture2.png", GL_REPEAT, GL_LINEAR);

    Program1.Bind();
    Program1.SetUniform1i("texture1", 0);
    Program1.SetUniform1i("texture2", 1);

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

    // light test
    BufferLayout layout2;
    vector<int> layout_stride2{3, 3};
    layout2.AddFloat(layout_stride2);
    VertexArray VAO2;
    VertexBuffer VBO2(vertices_cube, sizeof(vertices_cube));
    VAO2.AddBuffer(VBO2, layout2);

    // VertexArray VAO_light;
    // VAO_light.AddBuffer(VBO2, layout2);

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::vec3 lightSunlight_RGB(255.0f, 244.0f, 214.0f);
    glm::vec3 lightSunlight = lightSunlight_RGB / 255.0f;

    // glm::vec3 diffuseColor = lightColor * glm::vec3(0.7f);
    // glm::vec3 ambientColor = diffuseColor * glm::vec3(0.5);
    // Program_cubes.SetUniform3f("light.ambient", ambientColor);
    // Program_cubes.SetUniform3f("light.diffuse", diffuseColor);
    // Program_cubes.SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    // Program_cubes.SetUniform3f("material.ambient", 1.0f, 0.5f, 0.31f);
    // Program_cubes.SetUniform3f("material.diffuse", 1.0f, 0.5f, 0.31f);
    // Program_cubes.SetUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
    // Program_cubes.SetUniform1f("material.shininess", 32.0f);

    // Program_cubes.SetUniform3f("objectColor", 1.0f, 0.5f, 0.31f);

    // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.01f);
    glm::vec3 lightColor(1.0f);
    float lightColor_ambient = 1.0f;
    float lightColor_diffuse = 1.0f;
    glm::vec3 light_shininess(1.0f);
    glm::vec3 material_ambient(0.0f, 0.1f, 0.06f);
    glm::vec3 material_diffuse(0.0f, 0.50980392f, 0.50980392f);
    glm::vec3 material_specular(0.50196078f, 0.50196078f, 0.50196078f);
    float material_shininess = 32.0f;
    glm::vec3 cube_color(1.0f);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            // static float f = 0.0f;
            // static int counter = 0;
            ImGui::Begin("Material Settings"); // Create a window called "Hello, world!" and append into it.
            // ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
            // ImGui::Checkbox("Demo Window", &value);   // Edit bools storing our window open/close state

            // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("light color rgb", (float *)&lightColor); // Edit 3 floats representing a color
            ImGui::SliderFloat3("light color vec", (float *)&lightColor, 0.0f, 1.0f);
            ImGui::SliderFloat("lightColor_ambient", &lightColor_ambient, 0.0f, 1.0f);
            ImGui::SliderFloat("lightColor_diffuse", &lightColor_diffuse, 0.0f, 1.0f);
            ImGui::SliderFloat3("light_shininess vec", (float *)&light_shininess, 0.0f, 1.0f);
            ImGui::SliderFloat3("material_ambient vec", (float *)&material_ambient, 0.0f, 1.0f);
            ImGui::SliderFloat3("material_diffuse vec", (float *)&material_diffuse, 0.0f, 1.0f);
            ImGui::SliderFloat3("material_specular vec", (float *)&material_specular, 0.0f, 1.0f);
            // ImGui::SliderFloat("material_shininess", &material_shininess, 0.0f, 255.0f);
            if (ImGui::Button("material_shininess"))
            {
                material_shininess += 32;
                if (material_shininess > 256)
                    material_shininess = 32;
            }
            ImGui::Text("material_shininess = %f", material_shininess);
            ImGui::ColorEdit3("cube_color rgb", (float *)&cube_color);
            ImGui::SliderFloat3("cube_color vec", (float *)&cube_color, 0.0f, 1.0f);
            // ImGui::SliderFloat("material ambient", );

            // if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            // counter++;
            // ImGui::SameLine();
            // ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        Program1.Bind();
        glActiveTexture(GL_TEXTURE0);
        texture1.Bind();
        glActiveTexture(GL_TEXTURE1);
        texture2.Bind();

        Program1.SetUniform1f("scale", key_value);
        Program1.SetUniform1i("switchTexture", switchTexture);

        float radius = 2.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view;
        view = camera.GetViewMatrix();

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.m_Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // unsigned int modelLocation = glGetUniformLocation(Program1.GetID(), "model");
        // unsigned int viewLocation = glGetUniformLocation(Program1.GetID(), "view");
        // glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        // glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
        Program1.SetUniform4m("view", view);
        Program1.SetUniform4m("projection", projection);

        VAO1.Bind();
        // Program1.SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
        Program1.SetUniform3f("lightColor", lightSunlight);
        Program1.SetUniform3f("lightPos", lightPos);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = (i + 1) * glfwGetTime();
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.5f, 0.0f));
            glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            Program1.SetUniform3m("normalMatrix", normalMatrix);
            Program1.SetUniform3f("viewPos", camera.m_cameraPos);
            Program1.SetUniform4m("model", model);
            // glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // light test
        VAO2.Bind();
        // glm::vec3 lightColor;
        // lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0));
        // lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7));
        // lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3));
        // ---
        // lightColor = lightSunlight;
        Program_light.Bind();
        Program_light.SetUniform4m("view", view);
        Program_light.SetUniform4m("projection", projection);
        Program_light.SetUniform3f("lightColor", lightColor);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        // model = glm::translate(model, glm::vec3(camX, 2.0, camZ));
        model = glm::scale(model, glm::vec3(0.2f));
        Program_light.SetUniform4m("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        Program_cubes.Bind();
        glm::vec3 diffuseColor = lightColor * glm::vec3(lightColor_ambient);
        glm::vec3 ambientColor = lightColor * glm::vec3(lightColor_diffuse);
        Program_cubes.SetUniform3f("light.ambient", ambientColor);
        Program_cubes.SetUniform3f("light.diffuse", diffuseColor);
        Program_cubes.SetUniform3f("light.specular", light_shininess);

        Program_cubes.SetUniform3f("material.ambient", material_ambient);
        Program_cubes.SetUniform3f("material.diffuse", material_diffuse);
        Program_cubes.SetUniform3f("material.specular", material_specular);
        Program_cubes.SetUniform1f("material.shininess", material_shininess);

        Program_cubes.SetUniform3f("objectColor", cube_color);
        // Program_cubes.SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
        // Program_cubes.SetUniform3f("lightColor", lightSunlight);
        Program_cubes.SetUniform3f("light.position", lightPos);
        // Program_cubes.SetUniform3f("lightPos", glm::vec3(camX, 2.0, camZ));
        Program_cubes.SetUniform4m("view", view);
        Program_cubes.SetUniform4m("projection", projection);
        model = glm::mat4(1.0f);
        // model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(1.0f, 0.5f, 0.0f));
        // model = glm::translate(model, glm::vec3(-camX, 0.0, -camZ));
        // model = glm::rotate(model, -(float)(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        Program_cubes.SetUniform4m("model", model);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
        Program_cubes.SetUniform3f("viewPos", camera.m_cameraPos);
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

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
    //     altPressed = true;
    //     firstMouse = true;
    // }
    // else
    // {
    //     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //     altPressed = false;
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
        static float speed = 1.0;
        speed += 1.0;
        if (speed >= 10)
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
    if (!altPressed)
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
    if (!altPressed)
        camera.ProcessMouseScroll(yoffset);
}
