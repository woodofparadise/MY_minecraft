#ifndef ITEMSELECTION_H
#define ITEMSELECTION_H

#include "../render/Shader.h"
#include "../world/terrain.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 辅助函数：计算射线与下一个整数平面的交点
float int_bound(float s, float ds) 
{
    // s: 起点
    // ds: 射线方向
    bool sIsInt = std::floor(s) == s;
    if (ds < 0 && sIsInt) 
    {
        return 0.0f;
    }
    if (ds == 0)
    {
        return FLT_MAX;
    } 
    
    float t;
    if (ds > 0) 
    {
        t = (std::floor(s) + 1 - s) / ds;
    } 
    else 
    {
        t = (s - std::floor(s)) / -ds;
    }
    
    return t;
}

bool raycast_step(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Terrain& terrain, glm::ivec3& hitBlock, glm::ivec3& lastHitBlock) 
{
    // origin: 射线起点，即玩家头部位置
    // direction: 射线方向
    // maxDistance: 最远可选中方块距离
    // hitBlock: 选中的方块

    // 射线步进算法
    glm::ivec3 currentBlock = glm::ivec3(glm::floor(origin));
    glm::ivec3 step = glm::ivec3(
        direction.x > 0 ? 1 : -1,
        direction.y > 0 ? 1 : -1,
        direction.z > 0 ? 1 : -1
    );
    
    // 射线从当前位置到下一个整数坐标平面的时间(分为三个维度)
    glm::vec3 tMax = glm::vec3(
        int_bound(origin.x, direction.x),
        int_bound(origin.y, direction.y),
        int_bound(origin.z, direction.z)
    );
    
    // 后续到达的整数平面可以直接通过斜率计算
    glm::vec3 tDelta = glm::vec3(
        direction.x != 0 ? std::abs(1.0f / direction.x) : FLT_MAX,
        direction.y != 0 ? std::abs(1.0f / direction.y) : FLT_MAX,
        direction.z != 0 ? std::abs(1.0f / direction.z) : FLT_MAX
    );
    
    glm::ivec3 distance = glm::ivec3(0, 0, 0);
    
    while (max(distance.x, max(distance.y, distance.z)) <= maxDistance) 
    {
        if (terrain.get_block_type(glm::vec3(currentBlock)+glm::vec3(0.5f)) != AIR) 
        {
            hitBlock = currentBlock;
            return true;
        }
        lastHitBlock = currentBlock;
        // 步进至下一个方块，选择最小的步进距离(对应最近的方块)
        if (tMax.x < tMax.y) 
        {
            if (tMax.x < tMax.z) 
            {
                currentBlock.x += step.x;
                tMax.x += tDelta.x;
                distance.x += 1;
            } 
            else 
            {
                currentBlock.z += step.z;
                tMax.z += tDelta.z;
                distance.z += 1;
            }
        } 
        else 
        {
            if (tMax.y < tMax.z) 
            {
                currentBlock.y += step.y;
                tMax.y += tDelta.y;
                distance.y += 1;
            } 
            else 
            {
                currentBlock.z += step.z;
                tMax.z += tDelta.z;
                distance.z += 1;
            }
        }
    }
    
    return false;
}

// 全局变量，只初始化一次
static unsigned int selectionVAO = 0, selectionVBO = 0;
static bool selectionBoxInitialized = false;

// 渲染选中方块的轮廓
void render_selection_box(const glm::ivec3& blockPos, Shader& selectionShader, glm::mat4& projection, glm::mat4& view) 
{
    // 首次调用时初始化VAO和VBO
    if (!selectionBoxInitialized) 
    {
        // 创建一个标准大小的线框（中心在原点，大小为1x1x1）
        std::vector<glm::vec3> vertices = {
            // 底部（z=-0.05平面）
            glm::vec3(-0.05f, -0.05f, -0.05f), glm::vec3(1.05f, -0.05f, -0.05f),  // 前下边
            glm::vec3(1.05f, -0.05f, -0.05f), glm::vec3(1.05f, 1.05f, -0.05f),   // 右下边
            glm::vec3(1.05f, 1.05f, -0.05f), glm::vec3(-0.05f, 1.05f, -0.05f),   // 后下边
            glm::vec3(-0.05f, 1.05f, -0.05f), glm::vec3(-0.05f, -0.05f, -0.05f),  // 左下边

            // 顶部（z=1.05平面）
            glm::vec3(-0.05f, -0.05f, 1.05f), glm::vec3(1.05f, -0.05f, 1.05f),  // 前上边
            glm::vec3(1.05f, -0.05f, 1.05f), glm::vec3(1.05f, 1.05f, 1.05f),   // 右上边
            glm::vec3(1.05f, 1.05f, 1.05f), glm::vec3(-0.05f, 1.05f, 1.05f),   // 后上边
            glm::vec3(-0.05f, 1.05f, 1.05f), glm::vec3(-0.05f, -0.05f, 1.05f),  // 左上边

            // 连接底部和顶部的竖线
            glm::vec3(-0.05f, -0.05f, -0.05f), glm::vec3(-0.05f, -0.05f, 1.05f),  // 前左竖线
            glm::vec3(1.05f, -0.05f, -0.05f), glm::vec3(1.05f, -0.05f, 1.05f),  // 前右竖线
            glm::vec3(1.05f, 1.05f, -0.05f), glm::vec3(1.05f, 1.05f, 1.05f),   // 后右竖线
            glm::vec3(-0.05f, 1.05f, -0.05f), glm::vec3(-0.05f, 1.05f, 1.05f)   // 后左竖线
        };

        // 创建并绑定VAO和VBO
        glGenVertexArrays(1, &selectionVAO);
        glGenBuffers(1, &selectionVBO);
        
        glBindVertexArray(selectionVAO);
        glBindBuffer(GL_ARRAY_BUFFER, selectionVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        selectionBoxInitialized = true;
    }

    // 启用线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // glDisable(GL_DEPTH_TEST);
    
    // 使用线框着色器
    selectionShader.use();
    selectionShader.set_mat4("projection", projection);
    selectionShader.set_mat4("view", view);
    
    // 创建模型矩阵，将标准立方体变换到目标位置和大小
    glm::mat4 model = glm::mat4(1.0f);
    // 平移到方块位置
    model = glm::translate(model, glm::vec3(blockPos));
    
    selectionShader.set_mat4("model", model);
    
    // 渲染线框
    glBindVertexArray(selectionVAO);
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);
    
    // 恢复填充模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // glEnable(GL_DEPTH_TEST);
}

bool is_overlap_with_player(const glm::vec3& playerPos, const glm::ivec3& blockPos)
{
    glm::ivec3 currentBlock = glm::ivec3(glm::floor(playerPos));
    // cout << blockPos.x << " " << blockPos.y << " " << blockPos.z << endl;
    // cout << currentBlock.x << " " << currentBlock.y << " " << currentBlock.z << endl << endl;
    if(blockPos == currentBlock)
    {
        return true;
    }
    currentBlock.y = glm::floor(playerPos.y+0.8f);
    if(blockPos == currentBlock)
    {
        return true;
    }
    currentBlock.y = glm::floor(playerPos.y+1.6f);
    if(blockPos == currentBlock)
    {
        return true;
    }
    return false;
}

#endif