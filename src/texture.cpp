#include "texture.h"

Texture::Texture(const string &path, GLenum textureTarget, GLenum wrapMode, GLenum mapFilter, bool gammaCorrection)
{
    m_TextureTarget = textureTarget;
    glf()->glGenTextures(1, &m_TextureID);
    glf()->glBindTexture(GL_TEXTURE_2D, m_TextureID);

    ImageData image;
    if (LoadImageFile(path, image))
    {
        const int width = image.width, height = image.height, nrChannels = image.channels;
        const unsigned char *data = image.data();
        glf()->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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
        glf()->glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, dataformat, GL_UNSIGNED_BYTE, data);
        glf()->glGenerateMipmap(GL_TEXTURE_2D);
        glf()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode); // set texture wrapping to GL_REPEAT (default wrapping method)
        glf()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glf()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mapFilter);
        glf()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mapFilter);
    }
    else
    {
        std::cout << "Failed to load texture path: " << path << std::endl;
        exit(0);
    }
}

Texture::~Texture()
{
    glf()->glDeleteTextures(1, &m_TextureID);
}

void Texture::Bind()
{
    glf()->glActiveTexture(m_TextureTarget);
    glf()->glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::UnBind()
{
    glf()->glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int &Texture::GetTextureID()
{
    return m_TextureID;
}

unsigned int loadCubemap(vector<string> faces)
{
    unsigned int textureID;
    glf()->glGenTextures(1, &textureID);
    glf()->glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    glf()->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        ImageData image;
        if (LoadImageFile(faces[i], image))
        {
            const GLenum format = image.channels == 4 ? GL_RGBA : (image.channels == 1 ? GL_RED : GL_RGB);
            glf()->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data());
        }
        else
        {
            cout << "Cubemap texture failed to load at path: " << faces[i] << endl;
        }
    }
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glf()->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}