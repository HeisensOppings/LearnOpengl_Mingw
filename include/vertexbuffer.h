#ifndef __VERTEXBUFFER_H__
#define __VERTEXBUFFER_H__

#include <iostream>
#include <assert.h>
#include <vector>
#include "glad/glad.h"
#include "bufferlayout.h"

class VertexBuffer
{
private:
    unsigned int m_VBO;

public:
    VertexBuffer(const void *data, unsigned int size)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }
    ~VertexBuffer()
    {
        glDeleteBuffers(1, &m_VBO);
    }
    void Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    }
    void UnBind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};

#endif