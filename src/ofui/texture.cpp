#include <ofui/texture.h>

#include <iostream>

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace ofui;

Texture::Texture(const std::string filename)
    : m_filename(filename), m_imageWidth(-1), m_imageHeight(-1), m_textureId(0), m_loaded(false), m_channels(-1)
{}

Texture::~Texture()
{}

std::shared_ptr<Texture> Texture::create(const std::string filename)
{
    return std::make_shared<Texture>(filename);
}

void Texture::load()
{
    if (m_loaded)
    {
        return;
    }

    int m_channels;

    unsigned char *data = stbi_load(m_filename.c_str(), &m_imageWidth, &m_imageHeight, &m_channels, 0);

    if (data)
    {
        glGenTextures(1, &m_textureId);
        glBindTexture(GL_TEXTURE_2D, m_textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); // Same

        if (m_channels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_imageWidth, m_imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (m_channels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_imageWidth, m_imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        // glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        m_loaded = true;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
}

void Texture::unload()
{
    if (!m_loaded)
    {
        return;
    }
    glDeleteTextures(1, &m_textureId);
    m_loaded = false;
}

void Texture::bind()
{
    if (!m_loaded)
    {
        load();
    }
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::width() const
{
    return m_imageWidth;
}

int Texture::height() const
{
    return m_imageHeight;
}

bool Texture::loaded() const
{
    return m_loaded;
}

int Texture::channels() const
{
    return m_channels;
}

GLuint Texture::id() const
{
    return m_textureId;
}
