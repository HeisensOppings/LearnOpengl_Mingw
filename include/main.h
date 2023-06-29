#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;
const unsigned int SCR_WIDTH = 400;
const unsigned int SCR_HEIGHT = 400;

float key_value = 1.0;
float key_value_x = 0.0;
float key_value_y = 0.0;
int switchTexture = 0;
GLFWwindow *window = nullptr;

int opengl_init();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

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
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowPos(window, 300, 640);
    return 0;
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and height
    // will be significantly larger than specified on retina displays
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        int width, heigh;
        glfwGetFramebufferSize(window, &width, &heigh);
        framebuffer_size_callback(window, width, heigh);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        key_value -= 0.01f;
        if (key_value <= 0.0f)
            key_value = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        key_value += 0.01f;
        if (key_value >= 1.0f)
            key_value = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        key_value_x += 0.01f;
        if (key_value_x >= 1.0f)
            key_value_x = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        key_value_x -= 0.01f;
        if (key_value_x >= 1.0f)
            key_value_x = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        key_value_y += 0.01f;
        if (key_value_y >= 1.0f)
            key_value_y = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        key_value_y -= 0.01f;
        if (key_value_y >= 1.0f)
            key_value_y = 1.0f;
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
}

// This is a list of ANSI Escape Codes used to set text colors and formats:
// \033[0m: Resets all text attributes, including color and format.
// \033[1m: Sets text to bold.
// \033[3m: Sets text to italic.
// \033[4m: Sets text to underline.
// \033[30m: Sets text to black.
// \033[31m: Sets text to red.
// \033[32m: Sets text to green.
// \033[33m: Sets text to yellow.
// \033[34m: Sets text to blue.
// \033[35m: Sets text to magenta.
// \033[36m: Sets text to cyan.
// \033[37m: Sets text to white.
// \033[40m: Sets text background to black.
// \033[41m: Sets text background to red.
// \033[42m: Sets text background to green.
// \033[43m: Sets text background to yellow.
// \033[44m: Sets text background to blue.
// \033[45m: Sets text background to magenta.
// \033[46m: Sets text background to cyan.
// \033[47m: Sets text background to white.

// powershell file: Microsoft.PowerShell_profile.ps1
// #function prompt{... }, this is a PowerShell function used to customize the format and color of the PowerShell command prompt
// #In this function, the current path is obtained through $ExecutionContext.SessionState.Path.CurrentLocation,
// #and the last folder name in the current path is obtained using the Split-Path function. Then, the Write-Host function is used to output the command prompt with the folder name, and the color is set to green and purple.

// function prompt{
//     $path = "$($ExecutionContext.SessionState.Path.CurrentLocation)" $folder = Split - Path - Leaf $path Write - Host - NoNewline "$([char]27)[35m[$folder]$([char]27)[32m$ " return ""}

//     New -
//     Alias g g++