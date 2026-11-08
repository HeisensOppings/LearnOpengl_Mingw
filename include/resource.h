#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <string>
#include <map>

#include "glfuncs.h"
#include "texture.h"
#include "shader.h"
#include "vertexarray.h"
#include "appcontrol.h"

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

#endif