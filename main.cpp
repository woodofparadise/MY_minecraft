#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "src/core/game.h"

int main()
{
    glfwInit(); // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //使用核心模式

    bool gameInitStatus = true;
    Game game(gameInitStatus, 114514);  // 记录游戏中的所有状态和信息
    if(!gameInitStatus)
    {
        cout << "game init failed" << endl;
        return -1;
    }

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    game.game_loop();

    game.clear();

    glfwTerminate();
    return 0;
}

