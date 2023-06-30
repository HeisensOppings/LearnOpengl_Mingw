#ifndef __VERTEXARRAY_H__
#define __VERTEXARRAY_H__

#include <iostream>
#include <assert.h>
#include <vector>
#include "glad/glad.h"
#include "bufferlayout.h"
#include "vertexbuffer.h"

class VertexArray
{
private:
    unsigned int m_VAO;

public:
    VertexArray() { glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);
    }
    ~VertexArray() { glDeleteVertexArrays(1, &m_VAO); }
    void Bind() { glBindVertexArray(m_VAO); }
    void UnBind() { glDeleteVertexArrays(1, &m_VAO); }
    void AddBuffer(const VertexBuffer &vbo, const BufferLayout &layout)
    {
        Bind();
        vbo.Bind();
        const vector<VertexBufferElement> elements = layout.GetElements();
        unsigned int offset = 0;
        for (unsigned int i = 0; i< elements.size(); ++i)
        {
            const VertexBufferElement element = elements[i];
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (void *)(uintptr_t)(offset * element.GetSizeOfType(element.type)));
            offset += element.count;
        }
    }
};

#endif
