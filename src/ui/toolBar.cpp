#include <glad/glad.h>
#include "toolBar.h"

void toolBar::set_toolbar()
{
    bar.set_HUDitem(80.0f, 720.0f, glm::vec2(SCR_WIDTH/2.0f, 60.0f), "./Textures/widgets.png", glm::vec2(1.0f/256.0f, 235.0f/256.0f), glm::vec2(181.0f/256.0f, 255.0f/256.0f));
    for(int i = 0; i < 9; i++)
    {
        glm::vec2 texBegin(0.0f), texEnd(0.0f);
        if(toolbarBlock[i] != AIR)
        {
            glm::vec2 topLeft = get_icon_tex_coord(toolbarBlock[i]);
            texBegin = glm::vec2(topLeft.x, topLeft.y - 1.0f/16.0f);
            texEnd   = glm::vec2(topLeft.x + 1.0f/16.0f, topLeft.y);
        }
        blockBars[i].set_HUDitem(60.0f, 60.0f, glm::vec2(SCR_WIDTH/2.0f+80*(i-4), 60.0f), nullptr, texBegin, texEnd);
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
        if(toolbarBlock[i] == AIR) continue;
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
