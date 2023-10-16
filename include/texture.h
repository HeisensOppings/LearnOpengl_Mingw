#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#define STBI_WINDOWS_UTF8

#include <iostream>
#include <vector>
#include <string>
#include "stb_image.h"
#include <glad/glad.h>
using namespace std;

class Texture
{
private:
    unsigned int m_TextureID;
    unsigned int m_TextureTarget;

public:
    Texture(const string &path, GLenum textureTarget = GL_TEXTURE0, GLenum wrapMode = GL_REPEAT, GLenum mapFilter = GL_LINEAR, bool gammaCorrection = false);
    ~Texture();
    void Bind();
    void UnBind();
    unsigned int &GetTextureID();
};

unsigned int loadCubemap(vector<string> faces);
#endif