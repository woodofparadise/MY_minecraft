#ifndef BASIC_STRUCT_H
#define BASIC_STRUCT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 Texcoord;
    float     LightLevel = 15.0f;  // 天空光等级 0~15，默认满亮度
    float     BlockLight = 0.0f;   // 方块光等级 0~15（火把等发光方块）
};

struct Triangle
{
    Vertex Verteices[3];
};

#endif