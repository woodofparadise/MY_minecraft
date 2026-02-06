#ifndef HUDPAINTER_H
#define HUDPAINTER_H

#include "../render/texture.h"
#include "../render/Shader.h"
#include "../render/basic_struct.h"
#include "../core/preDefined.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class HUDitem
{
public:
    HUDitem(){};

    HUDitem(float hei, float len, glm::vec2 pos, char const* path, glm::vec2 TexcoordBegin = glm::vec2(0.0f, 0.0f), glm::vec2 TexcoordEnd = glm::vec2(1.0f, 1.0f));

    void set_HUDitem(float hei, float len, glm::vec2 pos, char const* path, glm::vec2 TexcoordBegin = glm::vec2(0.0f, 0.0f), glm::vec2 TexcoordEnd = glm::vec2(1.0f, 1.0f));

    void draw_item(Shader& HUDShader);

    void bind_item_texture(Shader& HUDShader, int ID);

    void clear()
    {
        HUDTexture.clear();
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (EBO != 0) glDeleteBuffers(1, &EBO);
    }

private:
    std::vector<unsigned int> indices;
    std::vector<Vertex> vertices;
    glm::mat4 orthoMatrix;
    Texture HUDTexture;
    unsigned int EBO, VAO, VBO;
    int texID;
};

#endif
