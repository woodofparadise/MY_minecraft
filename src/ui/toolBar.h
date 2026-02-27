#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "../render/texture.h"
#include "../render/Shader.h"
#include "../core/preDefined.h"
#include "HUDpainter.h"
#include "../world/block.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class toolBar
{
public:
    int selectedBlock = 0;                  // 标记当前选中的工具栏格子
    BLOCK_TYPE toolbarBlock[9]= {AIR, GRASS, STONE, SAND, WATER, SOIL, GLASS, TORCH, DIAMOND};

    toolBar(){};
    void set_toolbar();
    void bind_texture(Shader& HUDShader, int toolbarTexID);
    void draw_toolbar(Shader& HUDShader);
    void clear();


private:
    HUDitem blockBars[9];
    HUDitem selectedBars[9];
    HUDitem bar;
};

#endif
