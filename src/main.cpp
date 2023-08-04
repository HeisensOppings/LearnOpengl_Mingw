#include "main.h"

#define SCR_WIDTH_DEFAULT 800
#define SCR_HEIGHT_DEFAULT 600
int SCR_WIDTH = SCR_WIDTH_DEFAULT;
int SCR_HEIGHT = SCR_HEIGHT_DEFAULT;

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
        // Back face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        // Right face
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // Top face
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };

    float PlaneVertices[] = {
        // positions       // normals   // texture Coords
        10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
        -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
        -10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
        10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 2.0f, 2.0f,
        -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f};

    float GrassVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
        1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f, 0.0f};

    float FrameVertices[] = {
        // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f};

    float SkyboxVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    Shader::ShaderInit("E:/Project/OpenGL/src/shaders.glsl");
    Shader Program_cubes(0);
    Shader Program_plane(0);
    Shader Program_light(1);
    Shader Program_grass(2);
    Shader Program_windo(2);
    Shader Program_buffe(3);
    Shader Program_SkyBo(4);
    Shader Program_geome(5, 5, 0);
    Shader Program_norma(6, 6, 1);

    Texture diffuseMapCubes("./src/image/container.png", GL_REPEAT, GL_LINEAR, 0);
    Texture specularMapCubes("./src/image/container_specular.png", GL_REPEAT, GL_LINEAR, 1);
    Texture diffuseMapPlane("./src/image/Indoor_Ly_Build_Floor_03_T4_Diffuse.png", GL_REPEAT, GL_LINEAR, 2);
    Texture specularMapPlane("./src/image/Indoor_Ly_Build_Floor_03_T4_SMBE.png", GL_REPEAT, GL_LINEAR, 3);
    Texture diffuseMapGrass("./src/image/grass_inazuma1.png", GL_CLAMP_TO_EDGE, GL_LINEAR, 4);
    Texture diffuseMapWindo("./src/image/blending_transparent_window.png", GL_CLAMP_TO_EDGE, GL_LINEAR, 5);
    vector<std::string> faces{
        "./src/image/right.jpg",
        "./src/image/left.jpg",
        "./src/image/top.jpg",
        "./src/image/bottom.jpg",
        "./src/image/front.jpg",
        "./src/image/back.jpg"};
    // unsigned int cubemapTexture = loadCubemap(faces);

    Program_cubes.Bind();
    Program_cubes.SetUniform1i("material.diffuse", 0);
    Program_cubes.SetUniform1i("material.specular", 1);

    Program_plane.Bind();
    Program_plane.SetUniform1i("material.diffuse", 2);
    Program_plane.SetUniform1i("material.specular", 3);

    Program_grass.Bind();
    Program_grass.SetUniform1i("texture1", 4);

    Program_windo.Bind();
    Program_windo.SetUniform1i("texture1", 5);

    Program_SkyBo.Bind();
    Program_SkyBo.SetUniform1i("skybox", 3);

    Model ourModel1("./model/lisa/lisa.obj");
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
        glm::vec3(0.0f, 0.0f, 0.0f),
        // glm::vec3(-3.8f, 0.0f, -12.3f),
        glm::vec3(-1.0f, 0.0f, -1.0f),
        glm::vec3(-2.1f, 0.0f, -7.5f),
        glm::vec3(1.3f, 0.0f, -2.5f),
        // glm::vec3(1.5f, 0.0f, -2.5f),
        // glm::vec3(1.5f, 0.0f, -1.5f),
        glm::vec3(-1.5f, 0.0f, -2.0f),
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

    glm::vec3 background_color(0.1);
    float speed_move = 10.0f;

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
    // air   water ice   glass  diamond
    vector<float> refractive_index{1.00, 1.33, 1.309, 1.51, 2.42};
    float refractive_rate = 1.20;

    int material_shininess = 32;

    bool lighting_mode_camera = false;
    bool depth_test = false;

    Program_buffe.Bind();
    Program_buffe.SetUniform1i("screenTexture", 5);

    // framebuffer configuraion
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    // unsigned int textureColorbuffer;
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // use a single renderbuffer object for both a depth AND stencil buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    // now actually attach it
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Lighting Settings");
            // ImGui::Image((ImTextureID)(intptr_t)textureColorbuffer, ImVec2(500, 500), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
            ImGui::PushItemWidth(150);
            if (ImGui::Button("reflect"))
            {
                depth_test = !depth_test;
                if (depth_test)
                {
                    static int index = 0;
                    if (index >= (int)refractive_index.size())
                        index = 0;
                    refractive_rate = index++;
                    // ImGui::SliderFloat("zbuffer_near", &zbuffer_near, 0.01, 100.0);
                    // ImGui::SliderFloat("zbuffer_far", &zbuffer_far, zbuffer_near, 200.0);
                }
            }
            ImGui::SliderFloat("speed move", &speed_move, 0.1f, 20.f);
            ImGui::SliderFloat("reflect rate", &refractive_rate, 1.00, 3.00);
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
        camera.SetSpeed(speed_move);

        // bind to framebuffer and draw scene as we normally would to color texture
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        // enable depth testing (is disabled for rendering screen-space quad)
        glEnable(GL_DEPTH_TEST);
        // make sure we clear the framebuffer's content
        glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        // render
#if 1
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
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // -------------------------------------- cubes
        VAO_Cubes.Bind();
        diffuseMapCubes.Bind();
        specularMapCubes.Bind();
        Program_cubes.Bind();
        Program_cubes.SetUniform4m("view", view);
        Program_cubes.SetUniform4m("projection", projection);
        Program_cubes.SetUniform1f("scales", 1.0f);
        // direction light
        // Program_cubes.SetUniform1i("depth_test", (int)depth_test);
        // Program_cubes.SetUniform1f("zbuffer_near", zbuffer_near);
        // Program_cubes.SetUniform1f("zbuffer_far", zbuffer_far);
        // Program_cubes.SetUniform1f("refractive_rate", refractive_rate);
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
        for (unsigned int i = 0; i < 1; i++)
        {
            Program_cubes.Bind();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            // float angle = (i + 1) * 0.5;
            // model = glm::rotate(model, angle, glm::vec3(1.0f, 0.5f, 0.0f));
            glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
            Program_cubes.SetUniform3f("viewPos", camera.m_cameraPos);
            Program_cubes.SetUniform4m("model", model);
            // glDrawArrays(GL_TRIANGLES, 0, 36);
            // glDrawArrays(GL_POINTS, 0, 36);
        }

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        Program_cubes.SetUniform4m("model", model);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        Program_cubes.SetUniform3m("normalMatrix", normalMatrix);
        ourModel1.Draw(Program_cubes);

        // ----------------------------- geometry test
        // VAO_Cubes.Bind();
        // diffuseMapCubes.Bind();
        // specularMapCubes.Bind();
        Program_geome.Bind();
        Program_geome.SetUniform4m("view", view);
        Program_geome.SetUniform4m("projection", projection);
        Program_geome.SetUniform1f("scales", 1.0f);
        // direction light
        // Program_geome.SetUniform1i("depth_test", (int)depth_test);
        // Program_geome.SetUniform1f("zbuffer_near", zbuffer_near);
        // Program_geome.SetUniform1f("zbuffer_far", zbuffer_far);
        // Program_geome.SetUniform1f("refractive_rate", refractive_rate);
        Program_geome.SetUniform3f("dirLight.direction", sunlight_pos);
        Program_geome.SetUniform3f("dirLight.ambient", sunlight_color * light_am_di_sp_directional.x);
        Program_geome.SetUniform3f("dirLight.diffuse", sunlight_color * light_am_di_sp_directional.y);
        Program_geome.SetUniform3f("dirLight.specular", sunlight_color * light_am_di_sp_directional.z);
        // point light
        for (GLuint i = 0; i < 1; i++)
        {
            string number = to_string(i);
            Program_geome.SetUniform3f("pointLights[" + number + "].position", pointLightPositions[i]);
            Program_geome.SetUniform3f("pointLights[" + number + "].ambient", lightColor_point * light_am_di_sp_point.x);
            Program_geome.SetUniform3f("pointLights[" + number + "].diffuse", lightColor_point * light_am_di_sp_point.y);
            Program_geome.SetUniform3f("pointLights[" + number + "].specular", lightColor_point * light_am_di_sp_point.z);
            Program_geome.SetUniform1f("pointLights[" + number + "].constant", 1.0f);
            Program_geome.SetUniform1f("pointLights[" + number + "].linear", light_distance[light_distance_select_point][0]);
            Program_geome.SetUniform1f("pointLights[" + number + "].quadratic", light_distance[light_distance_select_point][1]);
        }
        // spotLight
        Program_geome.SetUniform3f("spotLight.position", lightPos);
        Program_geome.SetUniform3f("spotLight.direction", lightDirection);
        Program_geome.SetUniform3f("spotLight.ambient", lightColor_spot * light_am_di_sp_spot.x);
        Program_geome.SetUniform3f("spotLight.diffuse", lightColor_spot * light_am_di_sp_spot.y);
        Program_geome.SetUniform3f("spotLight.specular", lightColor_spot * light_am_di_sp_spot.z);
        Program_geome.SetUniform1f("spotLight.constant", 1.0f);
        Program_geome.SetUniform1f("spotLight.linear", light_distance[light_distance_select_spot][0]);
        Program_geome.SetUniform1f("spotLight.quadratic", light_distance[light_distance_select_spot][1]);
        Program_geome.SetUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        Program_geome.SetUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        Program_geome.SetUniform1i("material.shininess", material_shininess);

        // render the loaded model
        // house
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, -0.22f, 0.0f));
        // model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
        // Program_cubes.SetUniform4m("model", model);
        // ourModel3.Draw(Program_cubes);
        // lisa
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        Program_geome.SetUniform4m("model", model);
        Program_geome.SetUniform1f("time", glfwGetTime());
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        Program_geome.SetUniform3m("normalMatrix", normalMatrix);
        ourModel1.Draw(Program_geome);

        Program_norma.Bind();
        Program_norma.SetUniform4m("view", view);
        Program_norma.SetUniform4m("projection", projection);
        Program_norma.SetUniform1f("scales", 1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        Program_norma.SetUniform3m("normalMatrix", normalMatrix);
        Program_norma.SetUniform4m("model", model);
        Program_norma.SetUniform1f("time", glfwGetTime());
        ourModel1.Draw(Program_norma);

        // heita
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(-1.0f, -0.12f, 0.0f));
        // model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        // Program_cubes.SetUniform4m("model", model);
        // ourModel2.Draw(Program_cubes);
        // VAO_Cubes.Bind();
        // Program_geome.Bind();
        // Program_geome.SetUniform4m("view", view);
        // Program_geome.SetUniform4m("projection", projection);
        // Program_geome.SetUniform1f("time", glfwGetTime());
        // glDrawArrays(GL_POINTS, 0, 36);

        // ---------------------------------------------- plane
        VAO_Plane.Bind();
        diffuseMapPlane.Bind();
        specularMapPlane.Bind();
        Program_plane.Bind();
        Program_plane.SetUniform4m("view", view);
        Program_plane.SetUniform4m("projection", projection);
        Program_plane.SetUniform1f("scales", 2.0f);

        // direction light
        Program_plane.SetUniform1i("depth_test", (int)depth_test);
        // Program_plane.SetUniform1f("zbuffer_near", zbuffer_near);
        // Program_plane.SetUniform1f("zbuffer_far", zbuffer_far);
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
        model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0));
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        Program_plane.SetUniform3m("normalMatrix", normalMatrix);
        Program_plane.SetUniform3f("viewPos", camera.m_cameraPos);
        Program_plane.SetUniform4m("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ---------------------------------------------- grass
        // VAO_Grass.Bind();
        // diffuseMapGrass.Bind();
        // Program_grass.Bind();
        // Program_grass.SetUniform4m("view", view);
        // Program_grass.SetUniform4m("projection", projection);
        // for (unsigned int i = 0; i < vegetation.size(); i++)
        // {
        //     model = glm::mat4(1.0f);
        //     model = glm::translate(model, vegetation[i]);
        //     // glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        //     // Program_grass.SetUniform3m("normalMatrix", normalMatrix);
        //     // Program_grass.SetUniform3f("viewPos", camera.m_cameraPos);
        //     Program_grass.SetUniform4m("model", model);
        //     glDrawArrays(GL_TRIANGLES, 0, 6);
        // }

        // // windows

        // // distance update
        // map<float, glm::vec3> sorted;
        // for (unsigned int i = 0; i < windows.size(); ++i)
        // {
        //     float distance = glm::length(camera.m_cameraPos - windows[i]);
        //     sorted[distance] = windows[i];
        // }

        // diffuseMapWindo.Bind();
        // Program_windo.Bind();
        // Program_windo.SetUniform4m("view", view);
        // Program_windo.SetUniform4m("projection", projection);
        // for (map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        // {
        //     model = glm::mat4(1.0f);
        //     model = glm::translate(model, it->second);
        //     Program_windo.SetUniform4m("model", model);
        //     glDrawArrays(GL_TRIANGLES, 0, 6);
        // }

        // // draw skybox as last
        // glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        // Program_SkyBo.Bind();
        // view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        // Program_SkyBo.SetUniform4m("view", view);
        // Program_SkyBo.SetUniform4m("projection", projection);
        // // skybox cube
        // VAO_SkyBoxs.Bind();
        // glActiveTexture(GL_TEXTURE3);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0);
        // glDepthFunc(GL_LESS); // set depth function back to default
#endif

        // // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport((width - SCR_WIDTH) / 2, (height - SCR_HEIGHT) / 2, SCR_WIDTH, SCR_HEIGHT);

        // // disable depth test so screen-space quad isn't discarded due to depth test
        glDisable(GL_DEPTH_TEST);
        // // clear all relevent buffers
        // // glClear(GL_COLOR_BUFFER_BIT);
        Program_buffe.Bind();
        VAO_Frame.Bind();
        glActiveTexture(GL_TEXTURE0 + 5);
        // glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        Program_buffe.SetUniform1i("mode", 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // glViewport(0, 0, SCR_WIDTH / 5, SCR_HEIGHT / 5);
        // Program_buffe.SetUniform1i("mode", 1);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glViewport(0, SCR_HEIGHT / 5, SCR_WIDTH / 5, SCR_HEIGHT / 5);
        // Program_buffe.SetUniform1i("mode", 2);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glViewport(0, SCR_HEIGHT / 5 * 2, SCR_WIDTH / 5, SCR_HEIGHT / 5);
        // Program_buffe.SetUniform1i("mode", 3);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glViewport(0, SCR_HEIGHT / 5 * 3, SCR_WIDTH / 5, SCR_HEIGHT / 5);
        // Program_buffe.SetUniform1i("mode", 4);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glViewport(0, SCR_HEIGHT / 5 * 4, SCR_WIDTH / 5, SCR_HEIGHT / 5);
        // Program_buffe.SetUniform1i("mode", 5);
        // glDrawArrays(GL_TRIANGLES, 0, 6);

        // ImGui::Image((ImTextureID)(intptr_t)textureColorbuffer, ImVec2(SCR_WIDTH / 5, SCR_HEIGHT / 5), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
        // ImGui::End();

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

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CW);
    // if(gl_FrontFacing)
    // {
    // if(gl_FragCoord.x < 400)
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // else
    // FragColor = vec4(result, 1.0);
    // }
    // else
    // {
    // FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    // }

    // glEnable(GL_PROGRAM_POINT_SIZE);
    // glDrawArrays(GL_POINTS, 0, 36);
    // if (gl_VertexID == 0) {
    //     gl_PointSize = 10.0;
    // } else if (gl_VertexID == 1) {
    //     gl_PointSize = 20.0;
    // } else {
    //     gl_PointSize = 30.0;
    // }
    // gl_PointSize = gl_Position.z;

    // glDepthFunc(GL_LESS);
    // glEnable(GL_STENCIL_TEST);
    // glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

    loadCameraPosition(camera);

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
