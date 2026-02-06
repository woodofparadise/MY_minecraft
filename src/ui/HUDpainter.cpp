#include <glad/glad.h>
#include "HUDpainter.h"

HUDitem::HUDitem(float hei, float len, glm::vec2 pos, char const* path, glm::vec2 TexcoordBegin, glm::vec2 TexcoordEnd)
{
    set_HUDitem(hei, len, pos, path, TexcoordBegin, TexcoordEnd);
}

void HUDitem::set_HUDitem(float hei, float len, glm::vec2 pos, char const* path, glm::vec2 TexcoordBegin, glm::vec2 TexcoordEnd)
{
    vertices.resize(4);
    vertices[0].Position = glm::vec3(-len/2+pos.x, hei/2+pos.y, 0.0f);
    vertices[1].Position = glm::vec3(-len/2+pos.x, -hei/2+pos.y, 0.0f);
    vertices[2].Position = glm::vec3(len/2+pos.x, -hei/2+pos.y, 0.0f);
    vertices[3].Position = glm::vec3(len/2+pos.x, hei/2+pos.y, 0.0f);

    vertices[0].Texcoord = glm::vec2(TexcoordBegin.x, TexcoordEnd.y);
    vertices[1].Texcoord = TexcoordBegin;
    vertices[2].Texcoord = glm::vec2(TexcoordEnd.x, TexcoordBegin.y);
    vertices[3].Texcoord = TexcoordEnd;

    indices = std::vector<unsigned int>{0, 1, 2, 2, 3, 0};

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (size_t)(vertices.size() * sizeof(Vertex)), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Texcoord));
    glEnableVertexAttribArray(0);

    if(path != nullptr)
    {
       HUDTexture.load_texture(path);
    }
    return ;
}

void HUDitem::bind_item_texture(Shader& HUDShader, int ID)
{
    texID = ID;
    if(!HUDTexture.isInit)
    {
        return ;
    }
    HUDShader.use();
    glActiveTexture(GL_TEXTURE0+ID);
    glBindTexture(GL_TEXTURE_2D, HUDTexture.TextureID);
    glActiveTexture(GL_TEXTURE0);
}

void HUDitem::draw_item(Shader& HUDShader)
{
    glDisable(GL_DEPTH_TEST);
    HUDShader.use();
    HUDShader.set_int("textureUsed", texID);
    glm::mat4 orthoMatrix = glm::ortho(0.0f, SCR_WIDTH, 0.0f, SCR_HEIGHT, -1.0f, 1.0f);
    HUDShader.set_mat4("orthoMatrix", orthoMatrix);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}
