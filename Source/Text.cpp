#include <Text.hpp>
#include <OpenGL.hpp>
#include <Log.hpp>
#include <Globals.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/matrix_transform.hpp>

TextRenderer::TextRenderer(const std::string &font_path,float glyph_size)
{
    Init(font_path, glyph_size);
}

void TextRenderer::Init(const std::string& font_path, float glyph_size){
    m_FontPath = font_path;
    m_GlyphSize = glyph_size;

    m_Characters.resize(CH_NUM);
    m_Transforms.resize(CH_LIMIT);
    m_ToRender.resize(CH_LIMIT);

    if(FT_Init_FreeType(&m_FT)){
        LogError("Couldn't init FreeType Library");
    }

    if(FT_New_Face(m_FT, font_path.c_str(), 0, &m_Face)){
        LogError("Failed to load font");
        return;
    }else{
        FT_Select_Charmap(m_Face, ft_encoding_unicode);
        FT_Set_Pixel_Sizes(m_Face, glyph_size, glyph_size);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //disable byte-alignment restriction

        glGenTextures(1, &m_TextureArrayID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArrayID);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, glyph_size, glyph_size, CH_NUM,
                     0, GL_RED, GL_UNSIGNED_BYTE, 0);

        for(int ch = 0; ch < CH_NUM; ch++){
            if(FT_Load_Char(m_Face, ch, FT_LOAD_RENDER)){
                LogError("Failed to load Glyph");
                continue;
            }

            if(m_Face->glyph->bitmap.rows > glyph_size){
                #ifdef DEBUG
                    LogWarning("Glyph %c is too tall",ch);
                #endif
                continue;
            }

            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY,
                0, 0, 0, ch,
                m_Face->glyph->bitmap.width,
                m_Face->glyph->bitmap.rows, 1,
                GL_RED, GL_UNSIGNED_BYTE,
                m_Face->glyph->bitmap.buffer
            );

            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            Character character = {
                ch,
                glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
                glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
                (unsigned int)m_Face->glyph->advance.x
            };
            m_Characters[ch] = character;
        }
        FT_Done_Face(m_Face);
    }
    FT_Done_FreeType(m_FT);

    float vertices[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    m_GPUBuffer.Init(4, 2 * sizeof(float), 0);
    m_GPUBuffer.SetData(0, (void*) vertices, 4, sizeof(float) * 2);
    m_GPUBuffer.AddAttribute(2, GL_FLOAT, 2 * sizeof(float));

    m_Shader.Load("Resources/Shaders/Text.vert", "Resources/Shaders/Text.frag");

    glm::mat4 proj = glm::ortho(0.0f, (float) g_ScreenWidth, 0.0f, (float) g_ScreenHeight);
    m_Shader.Bind();
    m_Shader.SetUniformMat4fv("proj", proj);
    m_Shader.SetUniform1i("text", 0);
}

void TextRenderer::Deinit(){
    glDeleteTextures(1, &m_TextureArrayID);
    m_GPUBuffer.Free();
    m_Shader.Unload();
}

void TextRenderer::DrawText(const std::string& text, unsigned int x, unsigned int y,
                            float scale, glm::vec4 color){

    EnableColorBlend();
    DisableDepthTest();
    
    float originalX = x;

    m_Shader.Bind();
    m_Shader.SetUniform4fv("textColor", color, 1);

    BindTextureArray(m_TextureArrayID, 0);
    m_GPUBuffer.BindVBO();
    m_GPUBuffer.BindVAO();

    int workingIndex = 0;

    for(char c : text){
        if(c == '\0'){  // Null terminator
            break;
        }

        const Character& ch = m_Characters[c]; // retrieve character info

        if(c == '\n'){ // new line, just move down
            y -= (ch.size.y * 1.3f * scale);
            x = originalX;
        }else if(c == ' '){ // space, just move over
            x += ((ch.advance >> 6) * scale);
        }else{
            float xpos = x + (ch.bearing.x * scale);
            float ypos = y - ((m_GlyphSize - ch.bearing.y) * scale);

            m_Transforms[workingIndex] = glm::translate(glm::mat4(1.0f), glm::vec3(xpos, ypos, 0)) * 
                                         glm::scale(glm::mat4(1.0f), glm::vec3(m_GlyphSize * scale, m_GlyphSize * scale, 0.0f));
            m_ToRender[workingIndex] = ch.texID;

            x += ((ch.advance >> 6) * scale);

            workingIndex++;
            if(workingIndex == CH_LIMIT - 1){ // if we reach the limit, draw and reset
                Draw(workingIndex);
                workingIndex = 0;
            }
        }
    }
    Draw(workingIndex); // draw the remaining characters

    DisableColorBlend();
    EnableDepthTest();
}

void TextRenderer::Draw(int num_characters){
    if(num_characters > 0){
        m_Shader.SetUniformMat4fv("transforms", m_Transforms[0], num_characters);
        m_Shader.SetUniform1iv("letterMap", &m_ToRender[0], num_characters);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num_characters);
    }
}

static TextRenderer g_DefaultTextRenderer;

void InitTextRenderer(const std::string& font_path, float glyph_size){
    g_DefaultTextRenderer.Init(font_path, glyph_size);
}

void DeinitTextRenderer(){
    g_DefaultTextRenderer.Deinit();
}

void DrawText(const std::string& text, unsigned int x, unsigned int y, unsigned int size, glm::vec4 color){
    g_DefaultTextRenderer.DrawText(text, x, y, size, color);
}

std::string FormatText(const std::string& text, ...){
    va_list args;
    va_start(args, text);
    char buffer[256];
    vsnprintf(buffer, 256, text.c_str(), args);
    va_end(args);
    return std::string(buffer);
}