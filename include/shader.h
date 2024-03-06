#ifndef __SHADER_H__
#define __SHADER_H__

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

// #define GLSL_CODE_OUTPUT
#define CHECK_SHADER(shader, id)                                                                              \
    do                                                                                                        \
    {                                                                                                         \
        GLint success;                                                                                        \
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);                                                   \
        if (!success)                                                                                         \
        {                                                                                                     \
            GLchar infoLog[512];                                                                              \
            glGetShaderInfoLog(shader, 512, NULL, infoLog);                                                   \
            std::cout << "\033[37;41mERROR:: " << #shader << " ID:" << id << " ::COMPILATION_FAILED\033[0m\n" \
                      << infoLog << std::endl;                                                                \
            exit(1);                                                                                          \
        }                                                                                                     \
    } while (0)

#define CHECK_PROGRAM(program)                                                                 \
    do                                                                                         \
    {                                                                                          \
        GLint success;                                                                         \
        glGetProgramiv(program, GL_LINK_STATUS, &success);                                     \
        if (!success)                                                                          \
        {                                                                                      \
            GLchar infoLog[512];                                                               \
            glGetProgramInfoLog(program, 512, NULL, infoLog);                                  \
            std::cout << "\033[37;41mERROR:: " << #program << " ::COMPILATION_FAILED\033[0m\n" \
                      << infoLog << std::endl;                                                 \
            exit(1);                                                                           \
        }                                                                                      \
    } while (0)

// The contents of the glsl file must be separated by a swap symbol
class Shader
{
private:
    GLuint ProgramID;
    std::unordered_map<std::string, int> m_UniformLoationCache;
    static vector<string> vertexShaderSources;
    static vector<string> fragmentShaderSources;
    static vector<string> geometryShaderSources;

public:
    static bool CodeOutput;
    static void ShaderInit(const string filepath);
    Shader(unsigned int vertexShader_ID, unsigned int fragmentShader_ID, int geometryShader_ID);
    Shader(unsigned int vertexShader_ID, unsigned int fragmentShader_ID) : Shader(vertexShader_ID, fragmentShader_ID, -1) {}
    ~Shader() { glDeleteProgram(ProgramID); }
    inline void Bind() { glUseProgram(ProgramID); }
    void Unbind() const { glUseProgram(0); }
    static void ShaderClear();
    GLuint &GetID() { return ProgramID; }

    int GetUniformLocation(const std::string &name);
    void SetUniform1b(const std::string &name, bool value);
    void SetUniform1i(const std::string &name, int value);
    void SetUniform1f(const std::string &name, float value);
    void SetUniform2f(const std::string &name, float f0, float f1);
    void SetUniform2f(const std::string &name, const glm::vec2 &value);
    void SetUniform3f(const std::string &name, float f0, float f1, float f2);
    void SetUniform3f(const std::string &name, const glm::vec3 &value);
    void SetUniform4f(const std::string &name, float f0, float f1, float f2, float f3);
    void SetUniform4f(const std::string &name, const glm::vec4 &value);
    void SetUniform2m(const std::string &name, const glm::mat2 &mat);
    void SetUniform3m(const std::string &name, const glm::mat3 &mat);
    void SetUniform4m(const std::string &name, const glm::mat4 &mat);
};

#endif
