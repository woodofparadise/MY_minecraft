#include "block.h"

// 返回左上顶点在纹理图集中的坐标
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
        case(TORCH):
        {
            return glm::vec2((float)0.0f, (float)11.0f/16.0f);
        }
    }
    return glm::vec2(0.0f);
}

glm::vec2 get_icon_tex_coord(BLOCK_TYPE blockType)
{
    switch(blockType)
    {
        case GRASS: return get_tex_coord(GRASS, 1);  // 显示绿色顶面
        case WOOD:  return get_tex_coord(WOOD, 3);   // 显示树皮侧面
        default:    return get_tex_coord(blockType, 3);
    }
}

// bool transparentType[BLOCK_TYPE_NUM] = {true, false, false, false, true, false, false, true, false, false, false, false};

// bool is_transparent(BLOCK_TYPE blockType)
// {
//     return transparentType[blockType];
// }

bool is_transparent(BLOCK_TYPE blockType)
{
    if(blockType == AIR || blockType == WATER || blockType == GLASS || blockType == TORCH)
    {
        return true;
    }
    return false;
}

bool is_translucent(BLOCK_TYPE blockType)
{
    return blockType == WATER || blockType == GLASS;
}

int get_opacity(BLOCK_TYPE type)
{
    switch(type)
    {
        case AIR:    return 1;
        case TORCH:    return 1;
        case WATER:  return 2;
        case GLASS:  return 1;
        default:     return 16;  // 实心方块，光无法穿透
    }
    return 16;
}