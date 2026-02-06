#ifndef CAMERA_H
#define CAMERA_H

#include <algorithm>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


enum MOVE_MODE{
	FORWARD = 1,
	BACKWARD,
	LEFT,
	RIGHT
};


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    glm::vec3 cameraPos; // 摄像机位置
    glm::vec3 cameraTarget; // 目标位置
    glm::vec3 cameraFront; // 指向摄影机z轴正方向，所以和指向目标的向量相反
    glm::vec3 cameraUp; // 上向量
    glm::vec3 cameraRight; // 右向量
    float Yaw; // 偏航角，即以y轴为中心的旋转角
    float Pitch; // 俯仰角，即以x轴为中心的旋转角

    Camera(glm::vec3 position, glm::vec3 Up = glm::vec3(0, 1, 0), float width = 800, float height = 600, float fov = 45.0, float near = 0.1f, float far = 1000.0f);

    Camera()
    {
        init_camera(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    void init_camera(glm::vec3 position, glm::vec3 Up = glm::vec3(0, 1, 0), float near = 0.1f, float far = 1000.0f, float pitch = 0.0f, float yaw = 90.0f, float width = 800, float height = 600, float fov = 45.0);

    // 尝试了一种基于屏幕中心坐标的视角变换
    void process_mouse_movement_1(float xPos, float yPos);

    // 基于偏移的视角变换
    void process_mouse_movement_2(float xoffset, float yoffset);

    void process_mouse_scroll(float yOffset);

    void process_keyboard(int mode, float deltaTime);

    glm::mat4 get_view_matrix()
    {
        update_view();
        return viewMatrix;
    }

    glm::mat4 get_projection_matrix()
    {
        update_projection();
        return projectionMatrix;
    }

    void update_window(float width, float height)
    {
        screenWidth = width;
        screenHeight = height;
    }

    void set_position(glm::vec3 Position)
    {
        cameraPos = Position;
    }

private:
    void update_view();

    void update_projection()
    {
        projectionMatrix = glm::mat4(1.0f);
        projectionMatrix = glm::perspective(glm::radians(visualField), (float)screenWidth / (float)screenHeight, zNear, zFar);
    }

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    float screenWidth; // 屏幕宽度
    float screenHeight; // 屏幕高度
    float visualField; // 投影矩阵视野
    float zNear; // 近平面
    float zFar; // 远平面
};

#endif
