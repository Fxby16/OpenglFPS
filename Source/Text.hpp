#pragma once

#include <Shader.hpp>
#include <GPUBuffer.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm.hpp>

#include <vector>

inline constexpr unsigned int CH_LIMIT = 100;
inline constexpr unsigned int CH_NUM = 128;

class TextRenderer{
public:
    TextRenderer() = default;
    TextRenderer(const std::string& font_path, float glyph_size);
    ~TextRenderer() = default;

    void Init(const std::string& font_path, float glyph_size);
    void Deinit();

    void DrawText(const std::string& text, unsigned int x, unsigned int y, float scale, glm::vec4 color);

    inline float GetGlyphSize() const { return m_GlyphSize; }

private:

    void Draw(int num_characters);

    struct Character{
        unsigned int texID;     // ID of the glyph texture       
        glm::ivec2 size;        // Size of glyph
        glm::ivec2 bearing;     // Offset from baseline to left/top of glyph
        unsigned int advance;   // Horizontal offset to advance to next glyph
    };

    FT_Library m_FT;
    FT_Face m_Face;

    GPUBuffer m_GPUBuffer;
    Shader m_Shader;

    unsigned int m_TextureArrayID;
    std::vector<Character> m_Characters;
    std::vector<glm::mat4> m_Transforms;
    std::vector<int> m_ToRender;

    std::string m_FontPath;
    float m_GlyphSize;
};

extern void InitTextRenderer(const std::string& font_path, float glyph_size);
extern void DeinitTextRenderer();

extern void DrawText(const std::string& text, unsigned int x, unsigned int y, unsigned int size, glm::vec4 color);
extern std::string FormatText(const char* text, ...);