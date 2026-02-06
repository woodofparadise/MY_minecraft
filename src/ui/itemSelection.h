#ifndef ITEMSELECTION_H
#define ITEMSELECTION_H

#include "../render/Shader.h"
#include "../world/terrain.h"
#include <vector>
#include <cmath>
#include <cfloat>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 辅助函数：计算射线与下一个整数平面的交点
float int_bound(float s, float ds);

bool raycast_step(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Terrain& terrain, glm::ivec3& hitBlock, glm::ivec3& lastHitBlock);

// 全局变量声明（定义在itemSelection.cpp中）
extern unsigned int selectionVAO;
extern unsigned int selectionVBO;
extern bool selectionBoxInitialized;

// 渲染选中方块的轮廓
void render_selection_box(const glm::ivec3& blockPos, Shader& selectionShader, glm::mat4& projection, glm::mat4& view);

bool is_overlap_with_player(const glm::vec3& playerPos, const glm::ivec3& blockPos);

#endif
