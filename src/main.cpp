#include "main.h"

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool rightClick = true;
bool firstMouse = true;
bool controlKey = false;

// Camera camera;
GLFWwindow *window = nullptr;
Camera *camera = nullptr;
Scene *scene = nullptr;

int main()
{
    if (opengl_init())
        return -1;

    camera = new Camera(window);
    scene = new Scene(camera, SCR_WIDTH, SCR_HEIGHT);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        imgui_frame();

        scene->Update(deltaTime);
        scene->Render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    camera->saveCameraPosition(window);

    glfwTerminate();
    return 0;
}

int opengl_init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    // glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_FRAMEBUFFER_SRGB);
    // glEnable(GL_CULL_FACE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGuiIO &io = ImGui::GetIO();
    // (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    const char *glsl_version = "#version 420";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return 0;
}

void imgui_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("LearnOpenGL");
    imgui_window_focus = ImGui::IsWindowFocused();
    ImGui::PushItemWidth(140);
    ImGui::ColorEdit3(" point", (float *)&AppControl::irradiance_color);
    ImGui::Text("Pos: %0.2f,%0.2f,%0.2f", camera->m_cameraPos.x, camera->m_cameraPos.y, camera->m_cameraPos.z);
    ImGui::Text("(%.3f ms)(%.1f fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    if (ImGui::CollapsingHeader("Text"))
    {
        ImGui::SliderFloat2("ShadowOffset", (float *)&AppControl::text_shadow_offset, -5.0f, 5.0f);
        ImGui::PushItemWidth(40);
        ImGui::SliderFloat("Thickness", &AppControl::thickness, 0.4, 0.8);
        ImGui::SliderFloat("Softness", &AppControl::softness, 0.0, 0.5);
        ImGui::SliderFloat("OutlineThickness", &AppControl::outline_thickness, 0.4, 0.8);
        ImGui::SliderFloat("OutlineSoftness", &AppControl::outline_softness, 0.0, 0.1);
    }
    ImGui::End();
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
    if (!imgui_window_focus)
    {
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera->ProcessKeyBoard(MOVE_RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera->ProcessKeyBoard(MOVE_LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            camera->ProcessKeyBoard(MOVE_FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            camera->ProcessKeyBoard(MOVE_BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            camera->ProcessKeyBoard(MOVE_UP, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            camera->ProcessKeyBoard(MOVE_DOWN, deltaTime);
        }
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
    if (!rightClick && !imgui_window_focus)
    {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    static float speed_move = 2.0f;
    (void)window;
    (void)xoffset;
    if (!rightClick && controlKey)
    {
        camera->ProcessMouseScroll(yoffset);
    }
    else if (yoffset > 0)
    {
        speed_move += (speed_move < 1.0f) ? 0.1f : 0.5f;
        // speed_move += 0.1;
        camera->SetSpeed(speed_move);
    }
    else if (yoffset < 0)
    {
        speed_move -= (speed_move <= 1.0f) ? 0.1f : 0.5f;
        // speed_move -= 0.1;
        speed_move = std::max(speed_move, 0.1f);
        camera->SetSpeed(speed_move);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && !imgui_window_focus)
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
}
