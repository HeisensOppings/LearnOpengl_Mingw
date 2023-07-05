#ifndef __VERTEXBUFFER_H__
#define __VERTEXBUFFER_H__

#include <iostream>
#include <assert.h>
#include <vector>
#include "glad/glad.h"

class VertexBuffer
{
private:
    unsigned int m_VBO;

public:
    VertexBuffer(const void *data, unsigned int size);
    ~VertexBuffer();
    void Bind() const;
    void UnBind() const;
};

#endif