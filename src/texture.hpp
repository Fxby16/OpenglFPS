#pragma once

#include <string>

class TextureEx{
public:
    TextureEx(const std::string& path, bool flip = true);
    TextureEx(const std::string& path, const std::string& type, bool flip = true);
    TextureEx(const std::string& path, unsigned char* data, const std::string& type, unsigned int width, unsigned int height, bool flip = true);
    TextureEx() = default;
    ~TextureEx() = default;

    void Free();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    inline unsigned int GetID() const { return m_ID; }
    inline unsigned int GetWidth() const { return m_Width; }
    inline unsigned int GetHeight() const { return m_Height; }

    inline const std::string& GetPath() const { return m_Path; }
    inline const std::string& GetType() const { return m_Type; }

    void SetId(unsigned int id) { m_ID = id; }
    void SetPath(const std::string& path) { m_Path = path; }
    void SetType(const std::string& type) { m_Type = type; }

private:
    void Init(const std::string& path, bool flip, unsigned char* data = nullptr);

    unsigned int m_ID;
    int m_Width, m_Height, m_BPP;
    std::string m_Path;
    std::string m_Type = "";
};