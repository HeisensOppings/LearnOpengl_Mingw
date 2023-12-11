#ifndef __OBJECTRENDER_H__
#define __OBJECTRENDER_H__

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <map>

#include "vertexarray.h"
#include "texture.h"
#include "shader.h"
#include "resource.h"
#include "appcontrol.h"
#include "model.h"

using namespace glm;

class SKYObject
{
private:
    Shader &m_shader;
    VertexArray &m_VAO;
    unsigned int m_skymap;

public:
    SKYObject(Shader &s, VertexArray &vao, unsigned int skymap);
    void Render();
};

class PBRObject
{
private:
    Shader &m_shader;
    Model *m_model = nullptr;

public:
    static bool initialized;
    static unsigned int envCubemap;
    static unsigned int irradianceMap;
    static unsigned int prefilterMap;
    static unsigned int brdfLUTTexture;
    static void PBRInit(Shader &s, string hdr_map);
    PBRObject(Shader &s,string model_path);
    ~PBRObject() { delete m_model; }
    void Render(mat4 &model, vec3 camera_pos);
};

class TextObject
{
private:
    Shader &m_shader;
    u32string m_u32str;
    VertexArray *m_VAO = nullptr;
    // x -> linespec; y -> wordspec
    vec2 m_Typography = vec2(8.0, 0.0);

public:
    void
    LoadString(u32string u32str, vec2 screenPos, vec2 typoglaphy);
    TextObject(Shader &s, u32string u32str, vec2 screenPos, vec2 typoglaphy) : m_shader(s)
    {
        m_shader.Bind();
        m_shader.SetUniform1i("textTexture", 0);
        LoadString(u32str, screenPos, typoglaphy);
    }
    ~TextObject()
    {
        if (m_VAO != nullptr)
            delete m_VAO;
    }
    void Render(mat4 &model);
};

#endif