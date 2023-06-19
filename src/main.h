#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "link.h"
#include <iostream>
#define COMPILE_CHECK
using namespace std;
const unsigned int SCR_WIDTH = 400;
const unsigned int SCR_HEIGHT = 300;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// const char *vertexShaderSource = "#version 330 core\n"
//                                  "layout (location = 0) in vec3 aPos;\n"
//                                  "void main()\n"
//                                  "{\n"
//                                  "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//                                  "}\0";

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\n\0";

const char *fragmentShaderSource1 = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "FragColor=vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                    "}\n\0";

const char *fragmentShaderSource2 = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "FragColor=vec4(1.0f, 0.2f, 1.0f, 1.0f);\n"
                                    "}\n\0";

#define CHECK_SHADER_PROGRAM(shaderProgram)                             \
    do                                                                  \
    {                                                                   \
        GLint success;                                                  \
        GLchar infoLog[512];                                            \
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);        \
        if (!success)                                                   \
        {                                                               \
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);     \
            std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" \
                      << infoLog << std::endl;                          \
        }                                                               \
    } while (0)

#define CHECK_FRAGMENT_SHADER_COMPILE(shader)                            \
    do                                                                   \
    {                                                                    \
        GLint success;                                                   \
        GLchar infoLog[512];                                             \
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);              \
        if (!success)                                                    \
        {                                                                \
            glGetShaderInfoLog(shader, 512, NULL, infoLog);              \
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" \
                      << infoLog << std::endl;                           \
        }                                                                \
    } while (0)

#define CHECK_VERTEX_SHADER_COMPILE(shader)                            \
    do                                                                 \
    {                                                                  \
        GLint success;                                                 \
        GLchar infoLog[512];                                           \
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);            \
        if (!success)                                                  \
        {                                                              \
            glGetShaderInfoLog(shader, 512, NULL, infoLog);            \
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" \
                      << infoLog << std::endl;                         \
        }                                                              \
    } while (0)
