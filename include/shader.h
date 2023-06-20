#ifndef __SHADER_H__
#define __SHADER_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

enum SHADER_TYPE
{
    VERTEX_T = 0,
    FRAGMENT_T,
};

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

class Shader
{
    private:
        GLuint ProgramID;
        std::unordered_map<std::string, int> m_UniformLoationCache;
        static vector<string> vertexShaderSources;
        static vector<string> fragmentShaderSources;

    public:
        static void ShaderInit(const string filepath);
        Shader(unsigned int vertexShader_ID, unsigned int fragmentShader_ID);
        ~Shader() { glDeleteProgram(ProgramID); }
        void Bind() const { glUseProgram(ProgramID); }
        void Unbind() const { glUseProgram(0); }
        GLuint &GetID() { return ProgramID; }

        int GetUniformLocation(const std::string &name);
        void SetUniform1i(const std::string &name, int value);
        void SetUniform1f(const std::string &name, float value);
        void SetUniform4f(const std::string &name, float f0, float f1, float f2, float f3);
};

#endif
