#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <iostream>
#include <string>
#include "stb_image.h"
#include <glad/glad.h>
using namespace std;

class Texture
{
private:
    unsigned int m_TextureID;
    unsigned int m_GL_TextureID;

public:
    Texture(const string &path, GLenum wrapMode, GLenum mapFilter, unsigned int gl_TextureID);
    ~Texture();
    void Bind();
    void UnBind();
};

#endif