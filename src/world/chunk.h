#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "block.h"
#include "perlin_noise.h"
#include "../render/basic_struct.h"
#include <vector>

#define CHUNK_SIZE 64
#define CHUNK_HEIGHT 128

class Chunk
{
    private:
        std::vector<std::vector<std::vector<BLOCK_TYPE> > > chunkBlocks;
        std::vector<std::vector<int> > heightMap;
        std::vector<Vertex> vertices;

        // 预计算纹理坐标以减少函数调用
        glm::vec2* sideTexCoords; // 存储方块的侧面纹理坐标
        glm::vec2* topTexCoords;  // 存储方块的顶部纹理坐标
        glm::vec2* bottomTexCoords; // 存储方块的底部纹理坐标

        void create_face(Vertex& vertex1, Vertex& vertex2, Vertex& vertex3, Vertex& vertex4);

        void upload_data();

    public:
        std::vector<unsigned int> indices;
        unsigned int EBO = 0, VAO = 0, VBO = 0;
        bool isModified = false;                            // 标志区块内的数据是否发生变化

        Chunk(): VAO(0), VBO(0), EBO(0), isModified(false){};

        Chunk(PerlinNoise& perlinNoise, int x, int y);

        int get_height(int i, int j) const
        {
            return heightMap[CHUNK_SIZE-1-j][i];
        }

        BLOCK_TYPE get_block_type(int i, int j, int k) const
        {
            return chunkBlocks[CHUNK_SIZE-1-j][i][k];
        }

        void update_data(const Chunk* left, const Chunk* right, const Chunk* forward, const Chunk* back);

        bool set_block(int i, int j, int k, BLOCK_TYPE blockType);

        double generate_height(PerlinNoise& perlinNoise, double x, double z);

        // 禁用拷贝
        Chunk(const Chunk&) = delete;
        Chunk& operator=(const Chunk&) = delete;

        // 启用移动
        Chunk(Chunk&& other) noexcept;
        Chunk& operator=(Chunk&& other) noexcept;

        ~Chunk()
        {
            // // 释放前检查ID是否有效（0是安全的，glDelete会忽略）
            if (VAO != 0) glDeleteVertexArrays(1, &VAO);
            if (VBO != 0) glDeleteBuffers(1, &VBO);
            if (EBO != 0) glDeleteBuffers(1, &EBO);

            delete[] sideTexCoords;
            delete[] topTexCoords;  
            delete[] bottomTexCoords; 
        }
};

// 样条曲线：将 [-1,1] 的 continental 值映射到合理的基础高度
double spline_map_continental(double c);

#endif
