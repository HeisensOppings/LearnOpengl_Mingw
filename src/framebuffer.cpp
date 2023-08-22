#include "framebuffer.h"

FrameBuffer::FrameBuffer(GLenum frame_width, GLenum frame_height, GLenum textureType, GLenum textureTarget, GLboolean depthmap)
{
    mwidth = frame_width;
    mheight = frame_height;
    mtextureTarget = textureTarget;
    mtextureType = textureType;
    // create framebuffer
    glGenFramebuffers(1, &mframebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, mframebuffer_id);
    // create texture
    glGenTextures(1, &mtexture_id);
    if (depthmap)
    {
        if (mtextureType == GL_TEXTURE_2D)
        {
            glBindTexture(GL_TEXTURE_2D, mtexture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mwidth, mheight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            // attach texture
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mtexture_id, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        else if (mtextureType == GL_TEXTURE_CUBE_MAP)
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, mtexture_id);
            for (unsigned int i = 0; i < 6; ++i)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, mwidth, mheight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            // attach depth texture as FBO's depth buffer
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mtexture_id, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
    }
    else
    {
        if (mtextureType == GL_TEXTURE_2D)
        {
            glBindTexture(GL_TEXTURE_2D, mtexture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mwidth, mheight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mtexture_id, 0);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
        }
        else if (mtextureType == GL_TEXTURE_2D_MULTISAMPLE)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mtexture_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, mwidth, mwidth, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mtexture_id, 0);
            // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
            unsigned int rbo;
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            // use a single renderbuffer object for both a depth AND stencil buffer
            // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, mwidth, mheight);
            // attach texture
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::TextureBind()
{
    glActiveTexture(mtextureTarget);
    glBindTexture(mtextureType, mtexture_id);
}

void FrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mframebuffer_id);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, mwidth, mheight);
}