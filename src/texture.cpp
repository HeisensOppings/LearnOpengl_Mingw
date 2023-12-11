#include "texture.h"

Texture::Texture(const string &path, GLenum textureTarget, GLenum wrapMode, GLenum mapFilter, bool gammaCorrection)
{
    m_TextureTarget = textureTarget;
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum internalformat = 0;
        GLenum dataformat = 0;
        if (nrChannels == 1)
        {
            internalformat = dataformat = GL_RED;
        }
        else if (nrChannels == 3)
        {
            internalformat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataformat = GL_RGB;
        }
        else if (nrChannels == 4)
        {
            internalformat = gammaCorrection ? GL_SRGB : GL_RGBA;
            dataformat = GL_RGBA;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, dataformat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode); // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mapFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mapFilter);
    }
    else
    {
        std::cout << "Failed to load texture path: " << path << std::endl;
        exit(0);
    }
    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_TextureID);
}

void Texture::Bind()
{
    glActiveTexture(m_TextureTarget);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::UnBind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int &Texture::GetTextureID()
{
    return m_TextureID;
}

unsigned int loadCubemap(vector<string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannals;
    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannals, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            cout << "Cubemap texture failed to load at path: " << faces[i] << endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}