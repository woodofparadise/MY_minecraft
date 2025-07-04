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
};

struct Triangle
{
    Vertex Verteices[3];
};

#endif