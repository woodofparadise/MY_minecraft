#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "block.h"
#include "perlin_noise.h"
#include "../render/basic_struct.h"
#include <vector>
#include <queue>

#define CHUNK_SIZE 32
#define CHUNK_HEIGHT 128

// 邻居区块光照更新等级（高级别包含低级别的全部操作）
// NONE        : 无需更新
// VERTEX_ONLY : 仅刷新顶点光照（放置方块导致边界光照降低）
// PROPAGATE   : 正向传播 + 刷新顶点（破坏方块导致边界光照增加）
// FULL_RESET  : 全量重算 = 重置 + 正向传播 + 刷新顶点（放置方块导致邻居光照需要擦除）
enum LightUpdateLevel { NONE = 0, VERTEX_ONLY = 1, PROPAGATE = 2, FULL_RESET = 3 };

// ============ 坐标系映射说明 ============
//
// chunkBlocks[i][j][k] 的三个维度:
//   i ∈ [0, CHUNK_SIZE)    — 对应 Z 轴（反向）
//   j ∈ [0, CHUNK_SIZE)    — 对应 X 轴
//   k ∈ [0, CHUNK_HEIGHT)  — 对应 Y 轴（高度）
//
// 数组索引 → mesh 局部坐标:
//   mesh.x = j
//   mesh.y = k
//   mesh.z = CHUNK_SIZE - 1 - i
//
// mesh 局部坐标 → 数组索引:
//   i = CHUNK_SIZE - 1 - mesh.z
//   j = mesh.x
//   k = mesh.y
//
// mesh 局部坐标 → 世界坐标 (chunk_index_x, chunk_index_z 为区块索引):
//   world.x = mesh.x + chunk_index_x * CHUNK_SIZE - CHUNK_SIZE / 2
//   world.y = mesh.y
//   world.z = mesh.z + chunk_index_z * CHUNK_SIZE - CHUNK_SIZE / 2
//
// 世界坐标 → 数组索引:
//   i = CHUNK_SIZE - 1 - (world.z - chunk_index_z * CHUNK_SIZE + CHUNK_SIZE / 2)
//   j = world.x - chunk_index_x * CHUNK_SIZE + CHUNK_SIZE / 2
//   k = world.y
//
// ========================================

class Chunk
{
    private:
        std::vector<std::vector<std::vector<BLOCK_TYPE> > > chunkBlocks;
        std::vector<std::vector<int> > heightMap;
        std::vector<Vertex> vertices;
        std::vector<Vertex> verticesT;              // 透明方块顶点数据
        std::vector<glm::vec3> transparentFaceCenters; // 每个透明面片的中心（chunk局部空间）
        std::vector<std::vector<std::vector<short> > > blockLights;

        // 预计算纹理坐标以减少函数调用
        glm::vec2* sideTexCoords; // 存储方块的侧面纹理坐标
        glm::vec2* topTexCoords;  // 存储方块的顶部纹理坐标
        glm::vec2* bottomTexCoords; // 存储方块的底部纹理坐标

        // 六个面的顶点偏移和法线（相对于方块原点 (xPos, yPos, zPos)）
        // 顺序：Back(i-1), Forward(i+1), Left(j-1), Right(j+1), Down(k-1), Up(k+1)
        static const glm::vec3 faceVertexOffset[6][4];
        static const glm::vec3 faceNormal[6];

        void create_face(Vertex& vertex1, Vertex& vertex2, Vertex& vertex3, Vertex& vertex4);
        void create_face_transparent(Vertex& vertex1, Vertex& vertex2, Vertex& vertex3, Vertex& vertex4);

        void upload_data();
        void upload_data_transparent();

        BLOCK_TYPE get_neighbor_block(int i, int j, int k, int face,
            const Chunk* neighbours[4]) const;

        // 获取相邻方块的光照等级（用于 mesh 生成时写入顶点属性）
        float get_neighbor_light(int i, int j, int k, int face,
            const Chunk* neighbours[4]) const;

        // 光照 BFS 的通用传播函数（阶段一和阶段二共用）
        void update_block_light(std::queue<glm::ivec3>& lightBFS);

        // 数组空间的六邻居偏移 [i][j][k] 对应 [Z反向][X][Y高度]
        static const glm::ivec3 arrayOffset[6];

        // 破坏方块后的增量光照更新（正向BFS，可能标记邻居 PROPAGATE）
        void update_light_on_destroy(const glm::ivec3& index, Chunk* neighbours[4]);

        // 放置方块后的增量光照更新（反向BFS擦除 + 正向BFS回填，可能标记邻居 FULL_RESET / VERTEX_ONLY）
        void update_light_on_create(const glm::ivec3& index, Chunk* neighbours[4]);

        // 法线向量 → 面索引 (0~5)
        static int normal_to_face(const glm::vec3& normal);

    public:
        std::vector<unsigned int> indices;
        std::vector<unsigned int> indicesT;                 // 透明方块索引数据
        unsigned int EBO = 0, VAO = 0, VBO = 0;
        unsigned int transparentEBO = 0, transparentVAO = 0, transparentVBO = 0;
        bool isModified = false;                            // 标志区块内的数据是否发生变化（需要完整重建 mesh）
        LightUpdateLevel lightUpdate = NONE;                // 邻居区块光照更新等级

        Chunk(): VAO(0), VBO(0), EBO(0), transparentVAO(0), transparentVBO(0), transparentEBO(0), isModified(false){};

        Chunk(PerlinNoise& perlinNoise, int x, int y);

        int get_height(int i, int j) const
        {
            return heightMap[CHUNK_SIZE-1-j][i];
        }

        BLOCK_TYPE get_block_type(int i, int j, int k) const
        {
            return chunkBlocks[CHUNK_SIZE-1-j][i][k];
        }

        // neighbours[4] 顺序: {left(-X), right(+X), forward(-Z), back(+Z)}
        void update_data(const Chunk* neighbours[4]);

        void sort_transparent_faces(const glm::vec3& localCameraPos);

        bool set_block(int x, int y, int z, BLOCK_TYPE blockType, Chunk* neighbours[4]);

        double generate_height(PerlinNoise& perlinNoise, double x, double z);

        // 光照系统
        static bool is_valid_index(const glm::ivec3& index);
        short get_block_light(const glm::ivec3& index) const;
        void init_local_light();                          // 阶段一：区块内部光照
        void update_chunk_light(const Chunk* neighbours[4]); // 阶段二：跨区块边界传播

        // 仅刷新顶点的 LightLevel 并重传 VBO（不重建几何）
        void refresh_vertex_lights(const Chunk* neighbours[4]);

        // 禁用拷贝
        Chunk(const Chunk&) = delete;
        Chunk& operator=(const Chunk&) = delete;

        // 启用移动
        Chunk(Chunk&& other) noexcept;
        Chunk& operator=(Chunk&& other) noexcept;

        ~Chunk()
        {
            // 释放前检查ID是否有效（0是安全的，glDelete会忽略）
            if (VAO != 0) glDeleteVertexArrays(1, &VAO);
            if (VBO != 0) glDeleteBuffers(1, &VBO);
            if (EBO != 0) glDeleteBuffers(1, &EBO);
            if (transparentVAO != 0) glDeleteVertexArrays(1, &transparentVAO);
            if (transparentVBO != 0) glDeleteBuffers(1, &transparentVBO);
            if (transparentEBO != 0) glDeleteBuffers(1, &transparentEBO);

            delete[] sideTexCoords;
            delete[] topTexCoords;
            delete[] bottomTexCoords;
        }
};

// 样条曲线：将 [-1,1] 的 continental 值映射到合理的基础高度
double spline_map_continental(double c);

#endif
