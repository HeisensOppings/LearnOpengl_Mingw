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

public:
    Texture(const string &path, GLenum wrapMode, GLenum mapFilter);
    ~Texture();
    void Bind();
    void UnBind();
};

#endif