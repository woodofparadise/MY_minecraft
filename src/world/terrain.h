#ifndef TERRAIN_H
#define TERRAIN_H

#include "chunk.h"
#include "../render/Shader.h"
#include "../render/texture.h"
#include <map>
#include <memory>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Terrain
{
    private:
        map<pair<int, int>, unique_ptr<Chunk>> terrainMap;
        PerlinNoice perlinNoice;
        int chunk_index_x, chunk_index_z;
        Texture blockTexture;

    public:
        Terrain(){};

        Terrain(int seed, glm::vec3 position, char const* path)
        {
            init_terrain(seed, position, path);
        }

        void init_terrain(int seed, glm::vec3 position, char const* path)
        {
            perlinNoice.set_seed(seed);
            update_terrain(position);
            blockTexture.load_texture(path);
        }

        void update_terrain(glm::vec3 position);
        
        void bind_block_texture(Shader& blockShader)
        {
            blockShader.use();
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, blockTexture.TextureID);
            glActiveTexture(GL_TEXTURE0);
        }

        void draw_terrain(Shader& blockShader);

        int get_height(const glm::vec3& position);

        int get_block_type(const glm::vec3& position);

        bool destroy_block(glm::ivec3& selectedBlock);

        bool create_block(glm::ivec3& selectedBlock, BLOCK_TYPE blockType);

        void clear()
        {
            terrainMap.clear();
            blockTexture.clear();
        }
};

int Terrain::get_block_type(const glm::vec3& position)
{
    chunk_index_x = floor((float)(position.x+chunkSize/2) / (float)chunkSize);
    chunk_index_z = floor((float)(position.z+chunkSize/2) / (float)chunkSize);

    pair<int, int> index(chunk_index_x, chunk_index_z);
    if(terrainMap.find(index) == terrainMap.end())
    {
        terrainMap[index] = make_unique<Chunk>(perlinNoice, index.first, index.second);
    }
    return terrainMap[index]->get_block_type(position.x-chunk_index_x*chunkSize+chunkSize/2, position.z-chunk_index_z*chunkSize+chunkSize/2, position.y);
}

int Terrain::get_height(const glm::vec3& position)
{
    chunk_index_x = floor((float)(position.x+chunkSize/2) / (float)chunkSize);
    chunk_index_z = floor((float)(position.z+chunkSize/2) / (float)chunkSize);
    pair<int, int> index(chunk_index_x, chunk_index_z);
    if(terrainMap.find(index) == terrainMap.end())
    {
        terrainMap[index] = make_unique<Chunk>(perlinNoice, index.first, index.second);
    }
    return terrainMap[index]->get_height(position.x-chunk_index_x*chunkSize+chunkSize/2, position.z-chunk_index_z*chunkSize+chunkSize/2);
}

void Terrain::draw_terrain(Shader& blockShader)
{
    blockShader.set_int("textureUsed", 0);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, blockTexture.TextureID);
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            pair<int, int> index(chunk_index_x+i, chunk_index_z+j);
            // cout << index.first << " " << index.second << endl;
            glm::mat4 model = glm::mat4(1.0f);
            // z轴位移+1是为了修正chunk.h中构造函数计算与空气接触的面加入渲染队列的时候不小心整体后移了1，懒得调整回来了
            model = glm::translate(model, glm::vec3(index.first*chunkSize-chunkSize/2, 0.0f, index.second*chunkSize-chunkSize/2+1));
            blockShader.set_mat4("model", model);
            // model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            // model = glm::scale(model, glm::vec3(0.1f));
            glBindVertexArray(terrainMap[index]->VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainMap[index]->EBO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(terrainMap[index]->indices.size()), GL_UNSIGNED_INT, 0);
        }
    }
}

// 加载区块数据的时候，区块边界方块的渲染需要考虑相邻区块的情况
void Terrain::update_terrain(glm::vec3 position)
{
    chunk_index_x = floor((float)(position.x+chunkSize/2) / (float)chunkSize);
    chunk_index_z = floor((float)(position.z+chunkSize/2) / (float)chunkSize);
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            pair<int, int> index(chunk_index_x+i, chunk_index_z+j);
            if(terrainMap.find(index) == terrainMap.end())
            {
                terrainMap[index] = make_unique<Chunk>(perlinNoice, index.first, index.second);
            }
            if(terrainMap[index]->isModified)
            {
                for(int x = -1; x <= 1; x += 2)
                {
                    pair<int, int> adjIndex(index.first+x, index.second);
                    if(terrainMap.find(adjIndex) == terrainMap.end())
                    {
                        terrainMap[adjIndex] = make_unique<Chunk>(perlinNoice, adjIndex.first, adjIndex.second);
                    }
                }
                for(int y = -1; y <= 1; y += 2)
                {
                    pair<int, int> adjIndex(index.first, index.second+y);
                    if(terrainMap.find(adjIndex) == terrainMap.end())
                    {
                        terrainMap[adjIndex] = make_unique<Chunk>(perlinNoice, adjIndex.first, adjIndex.second);
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
    chunk_index_x = floor((float)(selectedBlock.x+chunkSize/2) / (float)chunkSize);
    chunk_index_z = floor((float)(selectedBlock.z+chunkSize/2) / (float)chunkSize);
    pair<int, int> index(chunk_index_x, chunk_index_z);
    if(terrainMap.find(index) == terrainMap.end())
    {
        terrainMap[index] = make_unique<Chunk>(perlinNoice, index.first, index.second);
    }
    return terrainMap[index]->set_block(selectedBlock.x-chunk_index_x*chunkSize+chunkSize/2, selectedBlock.z-chunk_index_z*chunkSize+chunkSize/2, selectedBlock.y, AIR);
}

bool Terrain::create_block(glm::ivec3& selectedBlock, BLOCK_TYPE blockType)
{
    chunk_index_x = floor((float)(selectedBlock.x+chunkSize/2) / (float)chunkSize);
    chunk_index_z = floor((float)(selectedBlock.z+chunkSize/2) / (float)chunkSize);
    pair<int, int> index(chunk_index_x, chunk_index_z);
    if(terrainMap.find(index) == terrainMap.end())
    {
        terrainMap[index] = make_unique<Chunk>(perlinNoice, index.first, index.second);
    }
    return terrainMap[index]->set_block(selectedBlock.x-chunk_index_x*chunkSize+chunkSize/2, selectedBlock.z-chunk_index_z*chunkSize+chunkSize/2, selectedBlock.y, blockType);
}

#endif