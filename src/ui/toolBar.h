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
    BLOCK_TYPE toolbarBlock[9]= {AIR, GRASS, STONE, SAND, WATER, SOIL, AIR, AIR, AIR};

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

void toolBar::set_toolbar()
{    
    bar.set_HUDitem(80.0f, 720.0f, glm::vec2(SCR_WIDTH/2.0f, 60.0f), "./Textures/widgets.png", glm::vec2(1.0f/256.0f, 235.0f/256.0f), glm::vec2(181.0f/256.0f, 255.0f/256.0f));
    for(int i = 0; i < 9; i++)
    {
        blockBars[i].set_HUDitem(60.0f, 60.0f, glm::vec2(SCR_WIDTH/2.0f+80*(i-4), 60.0f), nullptr);
        selectedBars[i].set_HUDitem(88.0f, 88.0f, glm::vec2(SCR_WIDTH/2.0f+80*(i-4), 60.0f), nullptr, glm::vec2(1.0f/256.0f, 211.0f/256.0f), glm::vec2(23.0f/256.0f, 233.0f/256.0f));
    }
    return ;
}

void toolBar::bind_texture(Shader& HUDShader, int toolbarTexID)
{
    HUDShader.use();
    bar.bind_item_texture(HUDShader, toolbarTexID);
    HUDShader.set_int("blockTexture", 1);
    HUDShader.set_int("toolbarTexture", toolbarTexID);
    for(int i = 0; i < 9; i++)
    {
        blockBars[i].bind_item_texture(HUDShader, 1);
        selectedBars[i].bind_item_texture(HUDShader, toolbarTexID);
    }
}

void toolBar::draw_toolbar(Shader& HUDShader)
{
    glDisable(GL_DEPTH_TEST);
    bar.draw_item(HUDShader);
    for(int i = 0; i < 9; i++)
    {
        HUDShader.set_int("blockType", toolbarBlock[i]);
        blockBars[i].draw_item(HUDShader);
    }
    selectedBars[selectedBlock].draw_item(HUDShader);
    glEnable(GL_DEPTH_TEST);
}

void toolBar::clear()
{
    for(int i = 0; i < 9; i++)
    {
        blockBars[i].clear();
        selectedBars[i].clear();
    }
    bar.clear();
}

#endif