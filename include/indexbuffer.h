#ifndef __INDEXBUFFER_H__
#define __INDEXBUFFER_H__

#include <iostream>
#include <assert.h>
#include <vector>
#include "glad/glad.h"
#include "bufferlayout.h"

class IndexBuffer
{
private:
    unsigned int m_IBO;
    unsigned int m_Count;

public:
    IndexBuffer(const void *data, unsigned int count):m_Count(count)
    {
        glGenBuffers(1, &m_IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
    }
    ~IndexBuffer()
    {
        glDeleteBuffers(1, &m_IBO);
    }
    void Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    }
    void UnBind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    inline unsigned int GetCount() const { return m_Count; }
};

#endif