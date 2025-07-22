#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "camera.h"
#include "itemSelection.h"
#include "collision.h"
#include "terrain.h"
#include "texture.h"
#include "player.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const float SCR_WIDTH = 800;
const float SCR_HEIGHT = 600;

class Game
{
    public:
        Player player;              // 主角
        Terrain terrain;            // 世界地图

        glm::ivec3 selectedBlock;   // 选中的方块

        Shader selectionShader;
        Shader blockShader;

        float deltaTime = 0.0f;         // 当前帧与上一帧的时间差
        float lastFrame = 0.0f;         // 上一帧的时间
        float lastX = SCR_WIDTH/2;      // 鼠标上一帧的横坐标
        float lastY = SCR_HEIGHT/2;     // 鼠标上一帧的纵坐标
        bool firstMouse = true;         // 标记鼠标是否是第一次进入游戏界面范围
        bool spaceKeyPressed = false;   // 标记空格键(控制跳跃)的按下状态
        bool RKeyPressed = false;       // 标记R键(控制视角切换)的按下状态
        int seed;                       // 世界种子
        GLFWwindow* window;             // 游戏窗口

        Game(bool& gameState, int seed = 666)
        {
            // 初始化世界种子
            this->seed = seed;

            // 设置长、宽、窗口名称
            window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MY_MINECRAFT", NULL, NULL);
            if(window == NULL)
            {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                gameState = false;
                return ;
            }
            glfwMakeContextCurrent(window); // 将我们窗口的上下文设置为当前线程的主上下文
            glfwSetWindowUserPointer(window, this); // 将游戏类的指针关联到游戏窗口上，方便窗口输入相关的静态函数访问
            
            set_wondow_properties();

            // 给GLAD传入用于加载系统相关OpenGL函数指针地址的函数
            if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                std::cout << "Fail to initialized GLAD" << std::endl;
                gameState = false;
                return ;
            }
            
            // 初始化着色器
            selectionShader.init_shader("./shaders/selectionShader.vs", "./shaders/selectionShader.fs");
            blockShader.init_shader("./shaders/blockShader.vs", "./shaders/blockShader.fs");

            // 初始化人物和地形
            blockShader.use();
            player.upload_data("./steve.png");
            terrain.init_terrain(this->seed, player.position, "./DefaultPack2.png");
            terrain.bind_block_texture(blockShader);
            player.bind_player_texture(blockShader);
            player.set_position(glm::vec3(0.5f, terrain.get_height(player.position), 0.5f));
            blockShader.set_int("blockTexture", 0);
            blockShader.set_int("playerTexture", 1);
            cout << "initialize success" << endl;
        }

        // 设置窗口属性，绑定发生窗口事件时调用的函数
        void set_wondow_properties()
        {
            // 注册帧缓冲大小函数，告诉GLFW每当窗口调整大小的时候调用该函数
            glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

            // 注册鼠标滚轮回调函数
            // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetScrollCallback(window, scroll_callback);

            // 注册鼠标事件回调函数
            glfwSetCursorPosCallback(window, mouse_callback);

            // 注册鼠标点击事件回调函数
            glfwSetMouseButtonCallback(window, mouse_button_callback);
        }

        // 游戏主循环
        void game_loop()
        {
            // 渲染循环，在每次循环开始前检查一次GLFW是否被要求退出
            while(!glfwWindowShouldClose(window))
            {
                // 输入
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;
                glfwSetCursorPos(window, SCR_WIDTH/2, SCR_HEIGHT/2);
                // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                process_input(window); // 在每一帧检测窗口是否返回
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置清空屏幕所用的颜色
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色缓冲和深度缓冲

                glm::mat4 view = player.camera.get_view_matrix();                // 观察矩阵
                glm::mat4 projection = player.camera.get_projection_matrix();    // 投影矩阵    
                // 检测选中的方块
                if (raycast_step(player.position+glm::vec3(0.0f, 1.6f, 0.0f), player.camera.cameraFront, 4.0f, terrain, selectedBlock)) 
                {
                    // 渲染选中效果
                    // cout << selectedBlock.x << " " << selectedBlock.y << " " << selectedBlock.z << endl;
                    render_selection_box(selectedBlock, selectionShader, projection, view);
                }
                
                blockShader.use();
                player.update_position(terrain, deltaTime);
                terrain.update_terrain(player.camera.cameraPos);               // 更新地形
                blockShader.set_mat4("view", view);
                blockShader.set_mat4("projection", projection);
                terrain.draw_terrain(blockShader);
                player.draw_player(blockShader);
                
                // 检查并调用事件，交换缓冲
                glfwSwapBuffers(window); // 交换颜色缓冲
                glfwPollEvents(); // 检查有无触发事件，比如键盘输入、鼠标移动
                // break;
            }
            return ;
        }

        // 释放资源
        void clear()
        {
            player.clear();
            terrain.clear();
            glfwDestroyWindow(window);
            cout << "Quiting game..." << endl;
        }

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        }

        static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
        {
            Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
            if (!game)
            {
                cout << "handle mouse movement error" << endl; 
                return ;
            }
            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);
            // cout << xpos << " " << ypos << endl;

            // player.camera.processMouseMovement1(xpos, ypos);

            // if(firstMouse)
            // {
            //     firstMouse = false;
            //     lastX = xpos;
            //     lastY = ypos;
            // }
            float xoffset = xpos - game->lastX;
            float yoffset = game->lastY - ypos;

            game->lastX = xpos;
            game->lastY = ypos;

            game->player.camera.process_mouse_movement_2(xoffset, yoffset);
        }

        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
        {
            Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
            if (!game)
            {
                cout << "handle scroll callback error" << endl; 
                return ;
            }
            game->player.camera.process_mouse_scroll(static_cast<float>(yoffset));
        }

        static void process_input(GLFWwindow *window)
        {
            Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
            if (!game)
            {
                cout << "handle keyboard input error" << endl; 
                return ;
            }
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                game->player.move(FORWARD, game->deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                game->player.move(BACKWARD, game->deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                game->player.move(LEFT, game->deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                game->player.move(RIGHT, game->deltaTime);
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
                game->player.switch_mode();
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !game->spaceKeyPressed) 
            {
                game->spaceKeyPressed = true;
                game->player.jump();
            } 
            else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) 
            {
                game->spaceKeyPressed = false;
            }
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, true);
            }
        }

        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
        {
            Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
            if (!game)
            {
                cout << "handle mouse input error" << endl; 
                return ;
            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
            {
                // 破坏方块
                game->terrain.destroy_block(game->selectedBlock);
            }    
        }
};

int main()
{
    glfwInit(); // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //使用核心模式

    bool gameInitStatus = true;
    Game game(gameInitStatus);  // 记录游戏中的所有状态和信息
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

