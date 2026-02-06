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
        PerlinNoise perlinNoise;
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
            perlinNoise.set_seed(seed);
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

        BLOCK_TYPE get_block_type(const glm::vec3& position);

        bool destroy_block(glm::ivec3& selectedBlock);

        bool create_block(glm::ivec3& selectedBlock, BLOCK_TYPE blockType);

        void clear()
        {
            terrainMap.clear();
            blockTexture.clear();
        }
};

#endif
