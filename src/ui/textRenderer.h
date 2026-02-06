#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>
#include <iostream>

#include "../core/preDefined.h"
#include "../render/Shader.h"

// 字符信息结构
struct Character {
    unsigned int TextureID;  // 字形纹理ID
    glm::ivec2   Size;       // 字形大小
    glm::ivec2   Bearing;    // 从基准线到字形左/上的偏移
    unsigned int Advance;    // 到下一个字形的水平偏移
};

class TextRenderer
{
public:
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;
    bool isInitialized = false;

    TextRenderer() : VAO(0), VBO(0) {}

    // 初始化FreeType并加载字体
    bool init(const std::string& fontPath, unsigned int fontSize = 24)
    {
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return false;
        }

        FT_Face face;
        if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
        {
            std::cout << "ERROR::FREETYPE: Failed to load font: " << fontPath << std::endl;
            FT_Done_FreeType(ft);
            return false;
        }

        // 设置字体大小
        FT_Set_Pixel_Sizes(face, 0, fontSize);

        // 禁用字节对齐限制
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // 加载ASCII字符的前128个
        for (unsigned char c = 0; c < 128; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYPE: Failed to load Glyph: " << c << std::endl;
                continue;
            }

            // 生成纹理
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            // 设置纹理参数
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }

        // 清理FreeType资源
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        // 配置VAO/VBO用于纹理四边形
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        isInitialized = true;
        return true;
    }

    // 渲染文本
    void renderText(Shader& shader, const std::string& text, float x, float y, float scale, glm::vec3 color)
    {
        if (!isInitialized) return;

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader.use();
        shader.set_vec3("textColor", color);
        glm::mat4 projection = glm::ortho(0.0f, SCR_WIDTH, 0.0f, SCR_HEIGHT);
        shader.set_mat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        for (auto c = text.begin(); c != text.end(); c++)
        {
            if (Characters.find(*c) == Characters.end()) continue;

            Character ch = Characters[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // 更新VBO
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            // 位移到下一个字符（advance以1/64像素为单位）
            x += (ch.Advance >> 6) * scale;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    // 清理资源
    void clear()
    {
        for (auto& pair : Characters)
        {
            glDeleteTextures(1, &pair.second.TextureID);
        }
        Characters.clear();

        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        VAO = 0;
        VBO = 0;
        isInitialized = false;
    }

    ~TextRenderer()
    {
        clear();
    }
};

#endif
