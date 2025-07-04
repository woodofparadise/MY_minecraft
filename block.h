#ifndef BLOCK_H
#define BLOCK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum BLOCK_TYPE
{
	AIR = 0,
	GRASS
};

struct Block
{
    int type;
};

#endif