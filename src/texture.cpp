#include <texture.hpp>
#include <log.hpp>

#include <stb_image.h>
#include <glad/glad.h>

Texture::Texture(const std::string& path, bool flip)
{
    Init(path, flip);
}

Texture::Texture(const std::string& path, const std::string& type, bool flip)
{
    Init(path, type, flip);
}

Texture::Texture(const std::string& path, unsigned char* data, const std::string& type, unsigned int width, unsigned int height, bool flip)
{
    Init(path, data, type, width, height, flip);
}

void Texture::Init(const std::string& path, bool flip)
{
    m_Path = path;

    printf("Texture without type %s\n", path.c_str());

    _Init(path, flip, nullptr);
}

void Texture::Init(const std::string& path, const std::string& type, bool flip)
{
    m_Path = path;
    m_Type = type;

    printf("Texture with type %s %s\n", path.c_str(), type.c_str());

    _Init(path, flip, nullptr);
}

void Texture::Init(const std::string& path, unsigned char* data, const std::string& type, unsigned int width, unsigned int height, bool flip)
{
    m_Path = path;
    m_Type = type;
    m_Width = width;
    m_Height = height;

    printf("Raw texture with type %s %s\n", path.c_str(), type.c_str());

    _Init(path, flip, data);
}

void Texture::_Init(const std::string& path, bool flip, unsigned char* data)
{
    LogMessage("Loading texture %s\n", path.c_str());

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(flip);

    unsigned char* local_buffer;
    
    if(data){
        local_buffer = data;
    }else{
        local_buffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 0);
    }

    if(local_buffer){
        GLenum format = GL_RGBA;

        switch(m_BPP){
            case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
            default: break;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, local_buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        printf("Failed to load texture\n");
    }

    stbi_image_free(local_buffer);
}

void Texture::Free()
{
    glDeleteTextures(1, &m_ID);
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}