#include <glad/glad.h>
#include "collision.h"

// 碰撞箱顶点数据（线框模式）
std::vector<glm::vec3>& get_AABBvertices() {
    static std::vector<glm::vec3> AABBvertices = {
    glm::vec3(0.0f, 0.0f, 0.0f), // 0: 左下后
    glm::vec3(1.0f, 0.0f, 0.0f), // 1: 右下后
    glm::vec3(1.0f, 1.0f, 0.0f), // 2: 右上后
    glm::vec3(0.0f, 1.0f, 0.0f), // 3: 左上后
    glm::vec3(0.0f, 0.0f, 1.0f), // 4: 左下前
    glm::vec3(1.0f, 0.0f, 1.0f), // 5: 右下前
    glm::vec3(1.0f, 1.0f, 1.0f), // 6: 右上前
    glm::vec3(0.0f, 1.0f, 1.0f)  // 7: 左上前
    };
    return AABBvertices;
}

// 线框连接顺序
std::vector<unsigned int>& get_AABBindices() {
    static std::vector<unsigned int> AABBindices = {
    0, 1, 1, 2, 2, 3, 3, 0, // 后面
    4, 5, 5, 6, 6, 7, 7, 4, // 前面
    0, 4, 1, 5, 2, 6, 3, 7  // 连接前后的边
    };
    return AABBindices;
}

bool check_collision(const AABB& player, const glm::ivec3& blockPos)
{
    AABB block(glm::vec3(blockPos)+glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f));
    return (player.minCoord.x < block.maxCoord.x && player.maxCoord.x > block.minCoord.x) &&
           (player.minCoord.y < block.maxCoord.y && player.maxCoord.y > block.minCoord.y) &&
           (player.minCoord.z < block.maxCoord.z && player.maxCoord.z > block.minCoord.z);
}

float calculate_overlap(const AABB& playerBox, const glm::ivec3& blockPos, int axis)
{
    // 计算玩家与方块在指定轴上的重叠量
    float blockMin = blockPos[axis];
    float blockMax = blockPos[axis] + 1.0f;
    float playerMin = playerBox.minCoord[axis];
    float playerMax = playerBox.maxCoord[axis];

    if (playerMin < blockMax && playerMax > blockMin)
    {
        // 计算两种可能的重叠方向
        float overlapLeft = blockMax - playerMin;
        float overlapRight = playerMax - blockMin;

        // 返回较小的重叠量
        return std::min(overlapLeft, overlapRight);
    }

    return 0.0f;
}

void AABB::draw_AABB(Shader& shader)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, get_AABBvertices().size() * sizeof(glm::vec3), &get_AABBvertices()[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, get_AABBindices().size() * sizeof(unsigned int), &get_AABBindices()[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader.use();

    // 设置模型矩阵（平移和缩放）
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, minCoord);
    model = glm::scale(model, maxCoord-minCoord);
    shader.set_mat4("model", model);

    // 启用线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 绘制碰撞箱
    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, get_AABBindices().size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 恢复填充模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void resolve_collisions(glm::vec3& position, const glm::vec3& playerSize, glm::vec3& velocity, AABB& playerBox, Terrain& terrain, float deltaTime)
{
    // 分别处理每个轴
    for (int axis = 0; axis < 3; ++axis)
    {
        axis = (axis+1)%3;
        float displacement = velocity[axis] * deltaTime;
        position[axis] += displacement;
        playerBox.update_AABB(position+glm::vec3(0.0f, 0.9f, 0.0f), playerSize);
        // 只检查可能碰撞的方块
        glm::ivec3 minBlock = glm::floor(playerBox.minCoord);
        glm::ivec3 maxBlock = glm::floor(playerBox.maxCoord);

        for (int y = minBlock.y; y <= maxBlock.y; ++y)
        {
            for (int x = minBlock.x; x <= maxBlock.x; ++x)
            {
                for (int z = minBlock.z; z <= maxBlock.z; ++z)
                {
                    glm::vec3 blockPos = glm::vec3(x, y, z);
                    if (terrain.get_block_type(blockPos) != AIR)
                    {
                        if (check_collision(playerBox, blockPos))
                        {
                            position[axis] -= displacement;
                            velocity[axis] = 0.0f;
                            y = maxBlock.y+1;
                            x = maxBlock.x+1;
                            z = maxBlock.z+1;
                        }
                    }
                }
            }
        }
    }
}
