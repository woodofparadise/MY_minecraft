#include "block.h"

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
            return glm::vec2((float)13.0f/16.0f, (float)4.0f/16.0f);
        }
        case(SOIL):
        {
            return glm::vec2((float)2.0f/16.0f, 1.0f);
        }
        case(WOOD):
        {
            if(face <= 2)
            {
                return glm::vec2(5.0f/16.0f, 15.0f/16.0f);
            }
            return glm::vec2(4.0f/16.0f, 15.0f/16.0f);
        }
        case(GLASS):
        {
            return glm::vec2(1.0f/16.0f, 13.0f/16.0f);
        }
        case(COAL):
        {
            return glm::vec2((float)2.0f/16.0f, (float)14.0f/16.0f);
        }
        case(IRON):
        {
            return glm::vec2((float)1.0f/16.0f, (float)14.0f/16.0f);
        }
        case(GOLD):
        {
            return glm::vec2(0.0f, (float)14.0f/16.0f);
        }
        case(DIAMOND):
        {
            return glm::vec2((float)2.0f/16.0f, (float)13.0f/16.0f);
        }
    }
    return glm::vec2(0.0f);
}

// bool transparentType[BLOCK_TYPE_NUM] = {true, false, false, false, true, false, false, true, false, false, false, false};

// bool is_transparent(BLOCK_TYPE blockType)
// {
//     return transparentType[blockType];
// }

bool is_transparent(BLOCK_TYPE blockType)
{
    if(blockType == AIR || blockType == WATER || blockType == GLASS)
    {
        return true;
    }
    return false;
}

int get_opacity(BLOCK_TYPE type)
{
    switch(type)
    {
        case AIR:    return 1;
        case WATER:  return 3;
        case GLASS:  return 1;
        default:     return 16;  // 实心方块，光无法穿透
    }
    return 16;
}