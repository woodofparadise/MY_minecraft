#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "block.h"
#include "perlin_noise.h"
#include "basic_struct.h"
#include <vector>

const int chunkSize = 32;
const int chunkHeight = 64;

class Chunk
{
    private:
        std::vector<std::vector<std::vector<unsigned int> > > chunkBlocks;
        std::vector<std::vector<int> > heightMap;
        std::vector<Vertex> vertices;

        glm::vec2 get_tex_coord(int blockType, int face);

        void create_face(Vertex& vertex1, Vertex& vertex2, Vertex& vertex3, Vertex& vertex4);

        void upload_data();

    public:
        std::vector<unsigned int> indices;
        unsigned int EBO, VAO, VBO;
        bool isModified;                            // 标志区块内的数据是否发生变化

        Chunk(){};

        Chunk(PerlinNoice& perlinNoice, int x, int y);

        int get_height(int i, int j)
        {
            return heightMap[chunkSize-1-j][i];
        }

        int get_block_type(int i, int j, int k)
        {
            return chunkBlocks[chunkSize-1-j][i][k];
        }

        void update_data(Chunk& left, Chunk& right, Chunk& forward, Chunk& back);

        bool set_block(int i, int j, int k, int blockType);


};

glm::vec2 Chunk::get_tex_coord(int blockType, int face)
{
    switch(blockType)
    {
        case(GRASS):{
                return glm::vec2((float)(face-1.0f)/16.0f, 1.0f);
        }
    }
    return glm::vec2(0.0f);
}

void Chunk::create_face(Vertex& vertex1, Vertex& vertex2, Vertex& vertex3, Vertex& vertex4)
{
    vertices.push_back(vertex1);
    vertices.push_back(vertex2);
    vertices.push_back(vertex3);
    vertices.push_back(vertex4);
    indices.push_back((unsigned int)vertices.size()-2);
    indices.push_back((unsigned int)vertices.size()-3);
    indices.push_back((unsigned int)vertices.size()-4);
    indices.push_back((unsigned int)vertices.size()-3);
    indices.push_back((unsigned int)vertices.size()-2);
    indices.push_back((unsigned int)vertices.size()-1);
    return ;
}

void Chunk::upload_data()
{
    vertices.shrink_to_fit();
    indices.shrink_to_fit();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (size_t)(vertices.size() * sizeof(Vertex)), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Texcoord));
    glEnableVertexAttribArray(0);
}

Chunk::Chunk(PerlinNoice& perlinNoice, int x, int y)
{
    double step = 1.0f/chunkSize;
    chunkBlocks.resize(chunkSize);
    heightMap.resize(chunkSize);

    // 基于二维柏林噪声生成随机地形
    for(int i = 0; i < chunkSize; i++)
    {
        chunkBlocks[chunkSize-1-i].resize(chunkSize);
        heightMap[chunkSize-1-i].resize(chunkSize);
        for(int j = 0; j < chunkSize; j++)
        {
            chunkBlocks[chunkSize-1-i][j].resize(chunkHeight, AIR);
            // cout << j << " " << perlinNoice.get_2D_perlin_noice((double)x+step*i, (double)y+step*j) << endl;
            int height = floor((double)chunkHeight/2 * max((perlinNoice.get_2D_perlin_noice((double)x+step*i+step/2, (double)y+step*j+step/2)+0.8), (double)0));
            heightMap[chunkSize-1-i][j] = height;
            for(int k = 0; k < height; k++)
            {
                chunkBlocks[chunkSize-1-i][j][k] = GRASS;
            }
        }
    }

    // 每个方块的索引即为其在该区块中的minCoord
    isModified = true;
}

void Chunk::update_data(Chunk& left, Chunk& right, Chunk& forward, Chunk& back)
{
    
    vector<Vertex>().swap(vertices);
    vector<unsigned int>().swap(indices);
    // 只渲染和空气方块接触的表面，其它表面隐藏
    for(int i = chunkSize-1; i >= 0; i--)
    {
        for(int j = 0; j < chunkSize; j++)
        {
            for(int k = 0; k < chunkHeight; k++)
            {
                // 跳过空气方块
                if(chunkBlocks[i][j][k] == AIR)
                {
                    continue;
                }
                // 后
                if((i-1 >= 0 && chunkBlocks[i-1][j][k] == AIR) || (i == 0  && back.get_block_type(j, 0, k) == AIR))
                {
                    Vertex vertex1; // 左上
                    vertex1.Position = glm::vec3(j, k+1, chunkSize-1-i);
                    vertex1.Normal = glm::vec3(-1, 0, 0);
                    vertex1.Texcoord = get_tex_coord(chunkBlocks[i][j][k], 2);
                    Vertex vertex2; // 右上
                    vertex2.Position = glm::vec3(j+1, k+1, chunkSize-1-i);
                    vertex2.Normal = glm::vec3(-1, 0, 0);
                    vertex2.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, 0.0f);
                    Vertex vertex3; // 左下
                    vertex3.Position = glm::vec3(j, k, chunkSize-1-i);
                    vertex3.Normal = glm::vec3(-1, 0, 0);
                    vertex3.Texcoord = vertex1.Texcoord+glm::vec2(0.0f, -1.0f/16.0f);
                    Vertex vertex4; // 右下
                    vertex4.Position = glm::vec3(j+1, k, chunkSize-1-i);
                    vertex4.Normal = glm::vec3(-1, 0, 0);
                    vertex4.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, -1.0f/16.0f);
                    create_face(vertex1, vertex2, vertex3, vertex4);
                }
                // 前
                if((i+1 < chunkSize && chunkBlocks[i+1][j][k] == AIR) || (i == chunkSize-1  && forward.get_block_type(j, chunkSize-1, k) == AIR))
                {
                    Vertex vertex1; // 左上
                    vertex1.Position = glm::vec3(j, k+1, chunkSize-1-(i+1));
                    vertex1.Normal = glm::vec3(1, 0, 0);
                    vertex1.Texcoord = get_tex_coord(chunkBlocks[i][j][k], 2);
                    Vertex vertex2; // 右上
                    vertex2.Position = glm::vec3(j+1, k+1, chunkSize-1-(i+1));
                    vertex2.Normal = glm::vec3(1, 0, 0);
                    vertex2.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, 0.0f);
                    Vertex vertex3; // 左下
                    vertex3.Position = glm::vec3(j, k, chunkSize-1-(i+1));
                    vertex3.Normal = glm::vec3(1, 0, 0);
                    vertex3.Texcoord = vertex1.Texcoord+glm::vec2(0.0f, -1.0f/16.0f);
                    Vertex vertex4; // 右下
                    vertex4.Position = glm::vec3(j+1, k, chunkSize-1-(i+1));
                    vertex4.Normal = glm::vec3(1, 0, 0);
                    vertex4.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, -1.0f/16.0f);
                    create_face(vertex1, vertex2, vertex3, vertex4);
                }
                // 左
                if((j-1 >= 0 && chunkBlocks[i][j-1][k] == AIR) || (j == 0  && left.get_block_type(chunkSize-1, chunkSize-1-i, k) == AIR))
                {
                    Vertex vertex1; // 左上
                    vertex1.Position = glm::vec3(j, k+1, chunkSize-1-i);
                    vertex1.Normal = glm::vec3(0, 0, -1);
                    vertex1.Texcoord = get_tex_coord(chunkBlocks[i][j][k], 2);
                    Vertex vertex2; // 右上
                    vertex2.Position = glm::vec3(j, k+1, chunkSize-1-(i+1));
                    vertex2.Normal = glm::vec3(0, 0, -1);
                    vertex2.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, 0.0f);
                    Vertex vertex3; // 左下
                    vertex3.Position = glm::vec3(j, k, chunkSize-1-i);
                    vertex3.Normal = glm::vec3(0, 0, -1);
                    vertex3.Texcoord = vertex1.Texcoord+glm::vec2(0.0f, -1.0f/16.0f);
                    Vertex vertex4; // 右下
                    vertex4.Position = glm::vec3(j, k, chunkSize-1-(i+1));
                    vertex4.Normal = glm::vec3(0, 0, -1);
                    vertex4.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, -1.0f/16.0f);
                    create_face(vertex1, vertex2, vertex3, vertex4);
                }
                // 右
                if((j+1 < chunkSize && chunkBlocks[i][j+1][k] == AIR) || (j == chunkSize-1 && right.get_block_type(0, chunkSize-1-i, k) == AIR))
                {
                    Vertex vertex1; // 左上
                    vertex1.Position = glm::vec3(j+1, k+1, chunkSize-1-i);
                    vertex1.Normal = glm::vec3(0, 0, 1);
                    vertex1.Texcoord = get_tex_coord(chunkBlocks[i][j][k], 2);
                    Vertex vertex2; // 右上
                    vertex2.Position = glm::vec3(j+1, k+1, chunkSize-1-(i+1));
                    vertex2.Normal = glm::vec3(0, 0, 1);
                    vertex2.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, 0.0f);
                    Vertex vertex3; // 左下
                    vertex3.Position = glm::vec3(j+1, k, chunkSize-1-i);
                    vertex3.Normal = glm::vec3(0, 0, 1);
                    vertex3.Texcoord = vertex1.Texcoord+glm::vec2(0.0f, -1.0f/16.0f);
                    Vertex vertex4; // 右下
                    vertex4.Position = glm::vec3(j+1, k, chunkSize-1-(i+1));
                    vertex4.Normal = glm::vec3(0, 0, 1);
                    vertex4.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, -1.0f/16.0f);
                    create_face(vertex1, vertex2, vertex3, vertex4);
                }
                if(k-1 >= 0 && chunkBlocks[i][j][k-1] == AIR)
                {
                    Vertex vertex1; // 左上
                    vertex1.Position = glm::vec3(j, k, chunkSize-1-i);
                    vertex1.Normal = glm::vec3(0, -1, 0);
                    vertex1.Texcoord = get_tex_coord(chunkBlocks[i][j][k], 3);
                    Vertex vertex2; // 右上
                    vertex2.Position = glm::vec3(j, k, chunkSize-1-(i+1));
                    vertex2.Normal = glm::vec3(0, -1, 0);
                    vertex2.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, 0.0f);
                    Vertex vertex3; // 左下
                    vertex3.Position = glm::vec3(j+1, k, chunkSize-1-i);
                    vertex3.Normal = glm::vec3(0, -1, 0);
                    vertex3.Texcoord = vertex1.Texcoord+glm::vec2(0.0f, -1.0f/16.0f);
                    Vertex vertex4; // 右下
                    vertex4.Position = glm::vec3(j+1, k, chunkSize-1-(i+1));
                    vertex4.Normal = glm::vec3(0, -1, 0);
                    vertex4.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, -1.0f/16.0f);
                    create_face(vertex1, vertex2, vertex3, vertex4);
                }
                if(k+1 < chunkHeight && chunkBlocks[i][j][k+1] == AIR)
                {
                    Vertex vertex1; // 左上
                    vertex1.Position = glm::vec3(j, k+1, chunkSize-1-i);
                    vertex1.Normal = glm::vec3(0, 1, 0);
                    vertex1.Texcoord = get_tex_coord(chunkBlocks[i][j][k], 1);
                    Vertex vertex2; // 右上
                    vertex2.Position = glm::vec3(j, k+1, chunkSize-1-(i+1));
                    vertex2.Normal = glm::vec3(0, 1, 0);
                    vertex2.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, 0.0f);
                    Vertex vertex3; // 左下
                    vertex3.Position = glm::vec3(j+1, k+1, chunkSize-1-i);
                    vertex3.Normal = glm::vec3(0, 1, 0);
                    vertex3.Texcoord = vertex1.Texcoord+glm::vec2(0.0f, -1.0f/16.0f);
                    Vertex vertex4; // 右下
                    vertex4.Position = glm::vec3(j+1, k+1, chunkSize-1-(i+1));
                    vertex4.Normal = glm::vec3(0, 1, 0);
                    vertex4.Texcoord = vertex1.Texcoord+glm::vec2(1.0f/16.0f, -1.0f/16.0f);
                    create_face(vertex1, vertex2, vertex3, vertex4);
                }
            }
        }
    }
    upload_data();
    isModified = false;
    return ;
}

bool Chunk::set_block(int i, int j, int k, int blockType)
{
    if(k < 0 || k >= chunkHeight)
    {
        return false;
    }
    isModified = true;
    chunkBlocks[chunkSize-1-j][i][k] = blockType;
    return true;
}

#endif