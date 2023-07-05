#ifndef __INDEXBUFFER_H__
#define __INDEXBUFFER_H__

#include <iostream>
#include <assert.h>
#include <vector>
#include "glad/glad.h"

class IndexBuffer
{
private:
    unsigned int m_IBO;
    unsigned int m_Count;

public:
    IndexBuffer(const void *data, unsigned int count);
    ~IndexBuffer();
    void Bind() const;
    void UnBind() const;
    inline unsigned int GetCount() const { return m_Count; }
};

#endif