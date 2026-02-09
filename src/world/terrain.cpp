#include <glad/glad.h>
#include "terrain.h"
#include <algorithm>

using namespace std;

// TODO: const glm::vec3 Chunk::terrainOffset[13][2] = {};

// 视锥体剔除：从VP矩阵提取6个平面，测试chunk的AABB是否完全在视锥外
bool Terrain::is_chunk_visible(const glm::mat4& vp, int cx, int cz) const
{
    // 从VP矩阵提取6个裁剪平面 (Gribb/Hartmann方法)
    // 平面方程: ax+by+cz+d >= 0 为可见侧
    // GLM列主序: vp[col][row]，手动提取行向量
    glm::vec4 planes[6];
    glm::vec4 row0(vp[0][0], vp[1][0], vp[2][0], vp[3][0]);
    glm::vec4 row1(vp[0][1], vp[1][1], vp[2][1], vp[3][1]);
    glm::vec4 row2(vp[0][2], vp[1][2], vp[2][2], vp[3][2]);
    glm::vec4 row3(vp[0][3], vp[1][3], vp[2][3], vp[3][3]);
    planes[0] = row3 + row0;  // Left
    planes[1] = row3 - row0;  // Right
    planes[2] = row3 + row1;  // Bottom
    planes[3] = row3 - row1;  // Top
    planes[4] = row3 + row2;  // Near
    planes[5] = row3 - row2;  // Far

    // chunk的世界空间AABB
    float minX = (float)(cx * CHUNK_SIZE - CHUNK_SIZE / 2);
    float minZ = (float)(cz * CHUNK_SIZE - CHUNK_SIZE / 2);
    glm::vec3 aabbMin(minX, 0.0f, minZ);
    glm::vec3 aabbMax(minX + CHUNK_SIZE, (float)CHUNK_HEIGHT, minZ + CHUNK_SIZE);

    // 对每个平面，用P-vertex测试
    for(int i = 0; i < 6; i++)
    {
        glm::vec3 p;
        p.x = (planes[i].x >= 0) ? aabbMax.x : aabbMin.x;
        p.y = (planes[i].y >= 0) ? aabbMax.y : aabbMin.y;
        p.z = (planes[i].z >= 0) ? aabbMax.z : aabbMin.z;
        if(glm::dot(glm::vec3(planes[i]), p) + planes[i].w < 0)
            return false;  // 完全在此平面外侧，不可见
    }
    return true;
}

BLOCK_TYPE Terrain::get_block_type(const glm::vec3& position)
{
    chunk_index_x = floor((float)(position.x+CHUNK_SIZE/2) / (float)CHUNK_SIZE);
    chunk_index_z = floor((float)(position.z+CHUNK_SIZE/2) / (float)CHUNK_SIZE);

    pair<int, int> index(chunk_index_x, chunk_index_z);
    if(terrainMap.find(index) == terrainMap.end())
    {
        terrainMap[index] = make_unique<Chunk>(perlinNoise, index.first, index.second);
    }
    return terrainMap[index]->get_block_type(position.x-chunk_index_x*CHUNK_SIZE+CHUNK_SIZE/2, position.z-chunk_index_z*CHUNK_SIZE+CHUNK_SIZE/2, position.y);
}

int Terrain::get_height(const glm::vec3& position)
{
    chunk_index_x = floor((float)(position.x+CHUNK_SIZE/2) / (float)CHUNK_SIZE);
    chunk_index_z = floor((float)(position.z+CHUNK_SIZE/2) / (float)CHUNK_SIZE);
    pair<int, int> index(chunk_index_x, chunk_index_z);
    if(terrainMap.find(index) == terrainMap.end())
    {
        terrainMap[index] = make_unique<Chunk>(perlinNoise, index.first, index.second);
    }
    return terrainMap[index]->get_height(position.x-chunk_index_x*CHUNK_SIZE+CHUNK_SIZE/2, position.z-chunk_index_z*CHUNK_SIZE+CHUNK_SIZE/2);
}

void Terrain::draw_terrain(Shader& blockShader, const glm::mat4& vpMatrix, const glm::vec3& cameraPos)
{
    blockShader.set_int("textureUsed", 0);
    unsigned int totalIndices = 0;

    // Pass 1: 不透明方块（深度写入ON，混合OFF）
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            pair<int, int> index(chunk_index_x+i, chunk_index_z+j);
            if(!is_chunk_visible(vpMatrix, index.first, index.second))
                continue;
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(index.first*CHUNK_SIZE-CHUNK_SIZE/2, 0.0f, index.second*CHUNK_SIZE-CHUNK_SIZE/2));
            blockShader.set_mat4("model", model);
            unsigned int indexCount = static_cast<unsigned int>(terrainMap[index]->indices.size());
            totalIndices += indexCount;
            glBindVertexArray(terrainMap[index]->VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainMap[index]->EBO);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        }
    }

    // Pass 2: 透明方块（深度写入OFF，混合ON，按距离从远到近排序）
    struct TransparentChunk {
        pair<int, int> index;
        float distSq;
    };
    vector<TransparentChunk> transparentChunks;

    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            pair<int, int> index(chunk_index_x+i, chunk_index_z+j);
            if(!is_chunk_visible(vpMatrix, index.first, index.second))
                continue;
            if(terrainMap[index]->indicesT.empty())
                continue;
            float cx = (float)(index.first * CHUNK_SIZE);
            float cz = (float)(index.second * CHUNK_SIZE);
            float dx = cx - cameraPos.x;
            float dz = cz - cameraPos.z;
            transparentChunks.push_back({index, dx*dx + dz*dz});
        }
    }

    sort(transparentChunks.begin(), transparentChunks.end(),
        [](const TransparentChunk& a, const TransparentChunk& b) { return a.distSq > b.distSq; });

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    for(auto& tc : transparentChunks)
    {
        glm::vec3 chunkOrigin(tc.index.first*CHUNK_SIZE-CHUNK_SIZE/2, 0.0f, tc.index.second*CHUNK_SIZE-CHUNK_SIZE/2);
        // 将摄像机变换到chunk局部空间，排序透明面片（远→近）
        terrainMap[tc.index]->sort_transparent_faces(cameraPos - chunkOrigin);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, chunkOrigin);
        blockShader.set_mat4("model", model);
        unsigned int indexCount = static_cast<unsigned int>(terrainMap[tc.index]->indicesT.size());
        totalIndices += indexCount;
        glBindVertexArray(terrainMap[tc.index]->transparentVAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainMap[tc.index]->transparentEBO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // 每个面 = 4顶点 + 6索引
    drawnTriangles = totalIndices / 3;
    drawnVertices = totalIndices / 6 * 4;
}

// 加载区块数据的时候，区块边界方块的渲染需要考虑相邻区块的情况
void Terrain::update_terrain(const glm::vec3& position, const glm::mat4* vpMatrix)
{
    chunk_index_x = floor((float)(position.x+CHUNK_SIZE/2) / (float)CHUNK_SIZE);
    chunk_index_z = floor((float)(position.z+CHUNK_SIZE/2) / (float)CHUNK_SIZE);
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            pair<int, int> index(chunk_index_x+i, chunk_index_z+j);
            if(terrainMap.find(index) == terrainMap.end())
            {
                terrainMap[index] = make_unique<Chunk>(perlinNoise, index.first, index.second);
            }
            if(terrainMap[index]->isModified)
            {
                // 有VP矩阵时跳过不可见chunk的网格构建，保留isModified等进入视野后再构建
                if(vpMatrix && !is_chunk_visible(*vpMatrix, index.first, index.second))
                    continue;

                for(int x = -1; x <= 1; x += 2)
                {
                    pair<int, int> adjIndex(index.first+x, index.second);
                    if(terrainMap.find(adjIndex) == terrainMap.end())
                    {
                        terrainMap[adjIndex] = make_unique<Chunk>(perlinNoise, adjIndex.first, adjIndex.second);
                    }
                }
                for(int y = -1; y <= 1; y += 2)
                {
                    pair<int, int> adjIndex(index.first, index.second+y);
                    if(terrainMap.find(adjIndex) == terrainMap.end())
                    {
                        terrainMap[adjIndex] = make_unique<Chunk>(perlinNoise, adjIndex.first, adjIndex.second);
                    }
                }
                pair<int, int> left(index.first-1, index.second); // 左
                pair<int, int> right(index.first+1, index.second); // 右
                pair<int, int> forward(index.first, index.second-1); // 前
                pair<int, int> back(index.first, index.second+1); // 后
                terrainMap[index]->update_data(terrainMap[left].get(), terrainMap[right].get(), terrainMap[forward].get(), terrainMap[back].get());
            }
        }
    }
}

bool Terrain::destroy_block(glm::ivec3& selectedBlock)
{
    chunk_index_x = floor((float)(selectedBlock.x+CHUNK_SIZE/2) / (float)CHUNK_SIZE);
    chunk_index_z = floor((float)(selectedBlock.z+CHUNK_SIZE/2) / (float)CHUNK_SIZE);
    pair<int, int> index(chunk_index_x, chunk_index_z);
    if(terrainMap.find(index) == terrainMap.end())
    {
        terrainMap[index] = make_unique<Chunk>(perlinNoise, index.first, index.second);
    }
    return terrainMap[index]->set_block(selectedBlock.x-chunk_index_x*CHUNK_SIZE+CHUNK_SIZE/2, selectedBlock.z-chunk_index_z*CHUNK_SIZE+CHUNK_SIZE/2, selectedBlock.y, AIR);
}

bool Terrain::create_block(glm::ivec3& selectedBlock, BLOCK_TYPE blockType)
{
    chunk_index_x = floor((float)(selectedBlock.x+CHUNK_SIZE/2) / (float)CHUNK_SIZE);
    chunk_index_z = floor((float)(selectedBlock.z+CHUNK_SIZE/2) / (float)CHUNK_SIZE);
    pair<int, int> index(chunk_index_x, chunk_index_z);
    if(terrainMap.find(index) == terrainMap.end())
    {
        terrainMap[index] = make_unique<Chunk>(perlinNoise, index.first, index.second);
    }
    return terrainMap[index]->set_block(selectedBlock.x-chunk_index_x*CHUNK_SIZE+CHUNK_SIZE/2, selectedBlock.z-chunk_index_z*CHUNK_SIZE+CHUNK_SIZE/2, selectedBlock.y, blockType);
}
