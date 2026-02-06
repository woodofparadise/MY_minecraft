#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../render/Shader.h"
#include "camera.h"
#include "../ui/itemSelection.h"
#include "../entity/collision.h"
#include "../world/terrain.h"
#include "../render/texture.h"
#include "../entity/player.h"
#include "preDefined.h"
#include "../ui/HUDpainter.h"
#include "../ui/textRenderer.h"
#include <glm/glm.hpp>
#include <sstream>
#include <iomanip>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Game
{
    public:
        Player player;              // 主角
        Terrain terrain;            // 世界地图

        glm::ivec3 selectedBlock;   // 选中的方块
        glm::ivec3 lastHitBlock;    // 可放置方块的位置(实际上就是步进算法直到selectedBlock前的最后一个空气方块)

        Shader selectionShader;
        Shader blockShader;
        Shader HUDShader;
        Shader textShader;

        TextRenderer textRenderer;
        float fpsUpdateInterval = 0.5f;  // FPS更新间隔（秒）
        float fpsAccumulator = 0.0f;     // 累计时间
        int frameCount = 0;              // 帧计数
        float currentFPS = 0.0f;         // 当前FPS

        std::vector<HUDitem> HUDitems;  // 游戏画面中的HUD元素，HUDitems[0]对应屏幕中心的光标

        float deltaTime = 0.0f;         // 当前帧与上一帧的时间差
        float lastFrame = 0.0f;         // 上一帧的时间
        float lastX = SCR_WIDTH/2;      // 鼠标上一帧的横坐标
        float lastY = SCR_HEIGHT/2;     // 鼠标上一帧的纵坐标
        bool firstMouse = true;         // 标记鼠标是否是第一次进入游戏界面范围
        bool blockSelected = false;     // 标记是否有效选中了一个方块
        int seed;                       // 世界种子
        GLFWwindow* window;             // 游戏窗口
        toolBar toolbar;                // 界面下方的工具栏

        Game(bool& gameState, int seed = 666);

        // 设置窗口属性，绑定发生窗口事件时调用的函数
        void set_wondow_properties();

        // 游戏主循环
        void game_loop();

        // 释放资源
        void clear();

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

        static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

        static void process_input(GLFWwindow *window);

        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

#endif
