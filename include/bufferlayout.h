#ifndef __BUFFERLAYOUT_H__
#define __BUFFERLAYOUT_H__

#include <iostream>
#include <assert.h>
#include <vector>
#include "glad/glad.h"

struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT:
            return sizeof(GLfloat);
        case GL_UNSIGNED_INT:
            return sizeof(GLuint);
        case GL_UNSIGNED_BYTE:
            return sizeof(GLbyte);
        }
        assert(false);
        return 0;
    }
};

class BufferLayout
{
private:
    unsigned int m_Stride;
    vector<VertexBufferElement> m_Elements;

    void Push(unsigned int type, unsigned int count, unsigned char normalized)
    {
        m_Elements.push_back({type, count, normalized});
        m_Stride += count * VertexBufferElement::GetSizeOfType(type);
    }

public:
    BufferLayout() : m_Stride(0) {}
    void AddFloat(const vector<int> layout_stride)
    {
        for (auto &stride : layout_stride)
            Push(GL_FLOAT, stride, GL_FALSE);
    }
    void AddFloat(unsigned int count) { Push(GL_FLOAT, count, GL_FALSE); }
    void AddUnsignedInt(unsigned int count) { Push(GL_UNSIGNED_INT, count, GL_FALSE); }
    void AddUnsignedByte(unsigned int count) { Push(GL_UNSIGNED_BYTE, count, GL_TRUE); }

    inline const vector<VertexBufferElement> GetElements() const { return m_Elements; }
    inline unsigned int GetStride() const { return m_Stride; }
};

#endif