#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <string>
#include <map>

#include <glad/glad.h>
#include "texture.h"
#include "shader.h"
#include "vertexarray.h"
#include "appcontrol.h"

#include "ft2build.h"
#include FT_FREETYPE_H

using namespace std;

class ResourceManager
{
public:
    static std::unordered_map<string, Shader> Shaders;
    static std::unordered_map<string, Texture> Textures;
    static std::unordered_map<string, VertexArray> VAOs;

    static void ShaderInit(string name);
    static void Clear();

    static Shader &SetShader(string name, int vertex, int fragment, int geometry = -1);
    static Shader &GetShader(std::string name);

    static void LoadTexture(const string name, const string file);
    static Texture &GetTexture(std::string name);

    static void SetVAO(string name, vector<unsigned int> bufferlayout, const void *vbo_data, unsigned int vbo_size, const int vbo_usage = GL_STATIC_DRAW,
                       const void *ibo_data = nullptr, unsigned int ibo_count = 0, const int ibo_usage = GL_STATIC_DRAW);
    static VertexArray &GetVAO(const string name);

private:
    ResourceManager() {}
};

extern vector<string> font_paths;
#define FONT_SIZE 30

struct Character
{
    glm::ivec2 Size;      // Size of glyph
    glm::ivec2 Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance; // Horizontal offset to advance to next glyph
    glm::vec4 Offset;     // textTexture offset
};

class TextTexture
{
public:
    static GLuint mtextureID;
    static GLfloat mtextureWidth, mtextureHeight;
    static map<char32_t, Character> Characters;
    static GLuint ChToTexture(u32string u32str);
    static GLuint GetTetureID() { return mtextureID; }
};

#endif