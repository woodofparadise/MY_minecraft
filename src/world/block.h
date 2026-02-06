#ifndef BLOCK_H
#define BLOCK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BLOCK_TYPE_NUM 12

enum BLOCK_TYPE
{
	AIR = 0,
	GRASS,
	STONE,
	SAND,
	WATER,
	SOIL,
	WOOD,
	GLASS,
	COAL,
	IRON,
	GOLD,
	DIAMOND
};

struct Block
{
    int type;
};

glm::vec2 get_tex_coord(unsigned int blockType, int face);

bool is_transparent(BLOCK_TYPE blockType);

#endif
