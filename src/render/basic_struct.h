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
    float     LightLevel = 15.0f;  // 编码光照：整数部分=天空光(0~15)，小数部分=方块光/16
};

struct Triangle
{
    Vertex Verteices[3];
};

#endif