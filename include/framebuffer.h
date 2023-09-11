#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include <iostream>
#include <glad/glad.h>
using namespace std;

enum TEXTURE_TYPE_ENUM
{
    TEXTURE_NORMAL_2D = 0,
    TEXTURE_DEPTH_2D,
    TEXTURE_DEPTH_CUBE
};

class FrameBuffer
{
public:
    GLenum mframebuffer_id;
    GLenum mtexture_id;
    GLenum mwidth, mheight;
    GLenum mtextureTarget;
    GLenum mtextureType;
    FrameBuffer(GLenum frame_width, GLenum frame_height, GLenum textureType = GL_TEXTURE_2D, GLenum textureTarget = GL_TEXTURE0, GLboolean depthmap = false);
    GLenum &GetTextureID() { return mtexture_id; }
    GLenum &GetFrameBufferID() { return mframebuffer_id; }
    void TextureBind();
    void Bind();
};

// class RenderBuffer
// {
//     RenderBuffer(GLenum frame_width, GLenum frame_height, vector<)
// };

#endif