#ifndef BLOCK_H
#define BLOCK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BLOCK_TYPE_NUM 13

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
	DIAMOND,
	TORCH
};

struct Block
{
    BLOCK_TYPE type;
};

glm::vec2 get_tex_coord(unsigned int blockType, int face);

// 返回方块在工具栏中显示的图标纹理左上角坐标
glm::vec2 get_icon_tex_coord(BLOCK_TYPE blockType);

// 可以透过此方块看到后面（邻居需要生成面片）
bool is_transparent(BLOCK_TYPE blockType);

// 需要 alpha 混合渲染（进透明 Pass）
bool is_translucent(BLOCK_TYPE blockType);

int get_opacity(BLOCK_TYPE type);

#endif
