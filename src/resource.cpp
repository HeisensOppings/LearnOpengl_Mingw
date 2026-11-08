#include "resource.h"

std::unordered_map<std::string, Texture> ResourceManager::Textures;
std::unordered_map<std::string, Shader> ResourceManager::Shaders;
std::unordered_map<std::string, VertexArray> ResourceManager::VAOs;

void ResourceManager::ShaderInit(string name)
{
    Shader::ShaderInit(name);
}

void ResourceManager::Clear()
{
    Shaders.erase(Shaders.begin(), Shaders.end());
    Textures.erase(Textures.begin(), Textures.end());
    VAOs.erase(VAOs.begin(), VAOs.end());
    // Shader::Clear();
}

Shader &ResourceManager::SetShader(string name, int vertex, int fragment, int geometry)
{
    auto it = Shaders.find(name);
    if (it == Shaders.end())
        return (Shaders.emplace(std::piecewise_construct,
                                std::forward_as_tuple(name), std::forward_as_tuple(vertex, fragment, geometry)))
            .first->second;
    else
        return (*it).second;
}

Shader &ResourceManager::GetShader(std::string name)
{
    try
    {
        return Shaders.at(name);
    }
    catch (const std::out_of_range &e)
    {
        cout << e.what() << ": " << name << endl;
        exit(1);
    }
}

void ResourceManager::LoadTexture(const string name, const string file)
{
    Textures.emplace(name, file);
}

Texture &ResourceManager::GetTexture(std::string name)
{
    try
    {
        return Textures.at(name);
    }
    catch (const std::out_of_range &e)
    {
        cout << e.what() << ": " << name << endl;
        exit(1);
    }
}

void ResourceManager::SetVAO(string name, vector<unsigned int> bufferlayout, const void *vbo_data, unsigned int vbo_size, const int vbo_usage,
                             const void *ibo_data, unsigned int ibo_count, const int ibo_usage)
{
    VAOs.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                 std::forward_as_tuple(bufferlayout, vbo_data, vbo_size, vbo_usage, ibo_data, ibo_count, ibo_usage));
}

VertexArray &ResourceManager::GetVAO(const string name)
{
    try
    {
        return VAOs.at(name);
    }
    catch (const std::out_of_range &e)
    {
        cout << e.what() << ": " << name << endl;
        exit(1);
    }
}
