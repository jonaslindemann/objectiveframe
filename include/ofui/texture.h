#pragma once

#include <memory>
#include <string>

#include <glad/glad.h>
#include <imgui.h>

namespace ofui {

class Texture {
private:
    std::string m_filename;
    int m_imageWidth;
    int m_imageHeight;
    GLuint m_textureId;
    bool m_loaded;
    int m_channels;

public:
    Texture(const std::string filename);
    virtual ~Texture();

    static std::shared_ptr<Texture> create(const std::string filename);

    void load();
    void unload();
    void bind();
    void unbind();

    int width() const;
    int height() const;

    bool loaded() const;
    int channels() const;

    GLuint id() const;
};

typedef std::shared_ptr<Texture> TexturePtr;

}; // namespace ofui
