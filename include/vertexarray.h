#ifndef __VERTEXARRAY_H__
#define __VERTEXARRAY_H__

#include <iostream>
#include <vector>
#include "glfuncs.h"

using namespace std;

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
        std::cout << __FUNCTION__ << "unknown type" << std::endl;
        exit(-1);
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
    BufferLayout(vector<unsigned int> vect) : m_Stride(0)
    {
        for (auto v : vect)
            Push(GL_FLOAT, v, GL_FALSE);
    }
    void AddUnsignedInt(unsigned int count) { Push(GL_UNSIGNED_INT, count, GL_FALSE); }
    void AddUnsignedByte(unsigned int count) { Push(GL_UNSIGNED_BYTE, count, GL_TRUE); }

    inline const vector<VertexBufferElement> GetElements() const { return m_Elements; }
    inline unsigned int GetStride() const { return m_Stride; }
};

class IndexBuffer
{
private:
    unsigned int m_Count;
    unsigned int m_IBO;

public:
    IndexBuffer(const void *data, unsigned int count, const int usage = GL_STATIC_DRAW) : m_Count(count)
    {
        glf()->glGenBuffers(1, &m_IBO);
        glf()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        glf()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count * sizeof(unsigned int), data, usage);
    };
    ~IndexBuffer() { glf()->glDeleteBuffers(1, &m_IBO); }
    void Bind() const { glf()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO); }
    void UnBind() const { glf()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
    inline unsigned int GetIboID() const { return m_IBO; }
    inline unsigned int GetCount() const { return m_Count; }
};

class VertexBuffer
{
private:
    unsigned int m_VBO;

public:
    VertexBuffer(const void *data, unsigned int size, const int usage = GL_STATIC_DRAW)
    {
        glf()->glGenBuffers(1, &m_VBO);
        glf()->glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glf()->glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }
    ~VertexBuffer() { glf()->glDeleteBuffers(1, &m_VBO); }
    void Bind() const { glf()->glBindBuffer(GL_ARRAY_BUFFER, m_VBO); }
    void UnBind() const { glf()->glBindBuffer(GL_ARRAY_BUFFER, 0); }
};

class VertexArray
{
private:
    unsigned int m_VAO;
    VertexBuffer m_VBO;
    IndexBuffer *m_IBO = nullptr;

public:
    VertexArray(vector<unsigned int> bufferlayout, const void *vbo_data, unsigned int vbo_size, const int vbo_usage = GL_STATIC_DRAW,
                const void *ibo_data = nullptr, unsigned int ibo_count = 0, const int ibo_usage = GL_STATIC_DRAW)
        : m_VBO(vbo_data, vbo_size, vbo_usage)
    {
        BufferLayout layout(bufferlayout);
        glf()->glGenVertexArrays(1, &m_VAO);
        glf()->glBindVertexArray(m_VAO);
        m_VBO.Bind();
        if (ibo_data != nullptr)
        {
            m_IBO = new IndexBuffer(ibo_data, ibo_count, ibo_usage);
            (*m_IBO).Bind();
        }
        const vector<VertexBufferElement> elements = layout.GetElements();
        unsigned int offset = 0;
        for (unsigned int i = 0; i < elements.size(); ++i)
        {
            const VertexBufferElement element = elements[i];
            glf()->glEnableVertexAttribArray(i);
            glf()->glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (void *)(uintptr_t)(offset * element.GetSizeOfType(element.type)));
            offset += element.count;
        }
    }
    ~VertexArray()
    {
        glf()->glDeleteVertexArrays(1, &m_VAO);
        if (m_IBO != nullptr)
            delete m_IBO;
    }
    void Bind() { glf()->glBindVertexArray(m_VAO); }
    void UnBind() { glf()->glDeleteVertexArrays(1, &m_VAO); }
    VertexBuffer &GetVertexBuffer() { return m_VBO; }
    IndexBuffer &GetIndexBuffer() { return *m_IBO; }
};

#endif
