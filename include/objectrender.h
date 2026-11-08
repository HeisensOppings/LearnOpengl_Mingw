#ifndef __OBJECTRENDER_H__
#define __OBJECTRENDER_H__

#include "glfuncs.h"
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
    static void PBRInit(Shader &s);
    PBRObject(Shader &s,string model_path);
    ~PBRObject() { delete m_model; }
    void Render(mat4 &model, vec3 camera_pos);
};

#endif