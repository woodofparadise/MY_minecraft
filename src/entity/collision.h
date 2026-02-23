#ifndef COLLISION_H
#define COLLISION_H

#include "../world/block.h"
#include "../world/terrain.h"
#include "../render/Shader.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 碰撞箱顶点数据（线框模式）
std::vector<glm::vec3>& get_AABBvertices();

// 线框连接顺序
std::vector<unsigned int>& get_AABBindices();

class AABB
{
    public:
    glm::vec3 minCoord;
    glm::vec3 maxCoord;
    unsigned int VAO, VBO, EBO;

    AABB()
    {}

    AABB(const glm::vec3& position, const glm::vec3& size)
    {
        update_AABB(position, size);
    }

    void update_AABB(const glm::vec3& position, const glm::vec3& size)
    {
        minCoord = position - size / 2.0f;
        maxCoord = position + size / 2.0f;
        return ;
    }


    // 绘制碰撞箱(测试版本)
    void draw_AABB(Shader& shader);
};

bool check_collision(const AABB& player, const glm::ivec3& blockPos);

float calculate_overlap(const AABB& playerBox, const glm::ivec3& blockPos, int axis);

void resolve_collisions(glm::vec3& position, const glm::vec3& playerSize, glm::vec3& velocity, AABB& playerBox, Terrain& terrain, float deltaTime);

#endif
