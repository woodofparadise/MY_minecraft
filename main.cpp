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

float SCR_WIDTH = 800;
float SCR_HEIGHT = 600;

// Camera camera(glm::vec3(0.0f, 80.0f, 0.0f));
// Camera camera(glm::vec3(-27.028, -3.23631, 4.8));
Player player;

glm::ivec3 selectedBlock; // 选中的方块


float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastX = SCR_WIDTH/2;
float lastY = SCR_HEIGHT/2;
float yaw = -90.0f, pitch = 0.0f;
float fov = 45.0f;
bool firstMouse = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
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
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    player.camera.processMouseMovement2(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    player.camera.processMouseScroll(static_cast<float>(yoffset));
}

bool spaceKeyPressed = false;
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        player.move(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        player.move(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.move(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.move(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        player.switchMode();
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spaceKeyPressed) 
    {
        spaceKeyPressed = true;
        player.jump();
    } 
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) 
    {
        spaceKeyPressed = false;
    }
}

int main()
{
    // xmlModel chair("./Product1");
    // return 0;
    glfwInit(); // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //使用核心模式
    
    // 设置长、宽、窗口名称
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CATIA", NULL, NULL);
    if(window == NULL)
    {
	    std::cout << "Failed to create GLFW window" << std::endl;
	    glfwTerminate();
	    return -1;
    }
    glfwMakeContextCurrent(window); // 将我们窗口的上下文设置为当前线程的主上下文
    
    // 注册帧缓冲大小函数，告诉GLFW每当窗口调整大小的时候调用该函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 注册鼠标滚轮回调函数
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);

    // 注册鼠标事件回调函数
    glfwSetCursorPosCallback(window, mouse_callback);

    // 给GLAD传入用于加载系统相关OpenGL函数指针地址的函数
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
	    std::cout << "Fail to initialized GLAD" << std::endl;
	    return -1;
    }

    Shader selectionShader("./shaders/selectionShader.vs", "./shaders/selectionShader.fs");

    Shader blockShader("./shaders/blockShader.vs", "./shaders/blockShader.fs");
    player.initGLResources("./steve.png");
    Terrain terrain(666, player.position, "./DefaultPack2.png");
    terrain.bindBlockTexture(blockShader);
    player.bindPlayerTexture(blockShader);
    player.setPosition(glm::vec3(0.5f, terrain.getHeight(player.position), 0.5f));
    blockShader.use();
    blockShader.setInt("blockTexture", 0);
    blockShader.setInt("playerTexture", 1);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_DEPTH_TEST);

    // 渲染循环，在每次循环开始前检查一次GLFW是否被要求退出
    while(!glfwWindowShouldClose(window))
    {
        // 输入
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwSetCursorPos(window, SCR_WIDTH/2, SCR_HEIGHT/2);
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        processInput(window); // 在每一帧检测窗口是否返回
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置清空屏幕所用的颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色缓冲和深度缓冲

        glm::mat4 view = player.camera.getViewMatrix();                // 观察矩阵
        glm::mat4 projection = player.camera.getProjectionMatrix();    // 投影矩阵    
        // 检测选中的方块
        if (raycast(player.position+glm::vec3(0.0f, 1.6f, 0.0f), player.camera.cameraFront, 4.0f, terrain, selectedBlock)) 
        {
            // 渲染选中效果
            // cout << selectedBlock.x << " " << selectedBlock.y << " " << selectedBlock.z << endl;
            renderSelectionBox(selectedBlock, selectionShader, projection, view);
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
        {
            // 破坏方块
            terrain.destroy_block(selectedBlock);
        }    
        
        blockShader.use();
        player.updatePosition(terrain, deltaTime);
        terrain.update_terrain(player.camera.cameraPos);               // 更新地形
        blockShader.setMat4("view", view);
        blockShader.setMat4("projection", projection);
        terrain.draw_terrain(blockShader);
        player.drawPlayer(blockShader);
        
        // 检查并调用事件，交换缓冲
	    glfwSwapBuffers(window); // 交换颜色缓冲
	    glfwPollEvents(); // 检查有无触发事件，比如键盘输入、鼠标移动
        // break;
    }

    glfwTerminate();
    return 0;
}

