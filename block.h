#ifndef BLOCK_H
#define BLOCK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

glm::vec2 get_tex_coord(unsigned int blockType, int face)
{
    // face = 1/2/3(上面/下面/侧面)
    switch(blockType)
    {
        case(GRASS):
        {
            if(face == 1)
            {
                return glm::vec2(0.0f, 1.0f);
            }
            return glm::vec2((float)(face)/16.0f, 1.0f);
        }
        case(STONE):
        {
            return glm::vec2((float)1.0f/16.0f, 1.0f);
        }
        case(SAND):
        {
            return glm::vec2((float)2.0f/16.0f, 1.0f-(float)1.0f/16.0f);
        }
        case(WATER):
        {
            return glm::vec2((float)13.0f/16.0f, 1.0f-(float)12.0f/16.0f);
        }
        case(SOIL):
        {
            return glm::vec2((float)2.0f/16.0f, 1.0f);
        }
    }
    return glm::vec2(0.0f);
}

#endif