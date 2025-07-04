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

    Camera(glm::vec3 position, glm::vec3 Up = glm::vec3(0, 1, 0), float width = 800, float height = 600, float fov = 45.0, float near = 0.1f, float far = 1000.0f);

    Camera()
    {
        InitCamera(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    void InitCamera(glm::vec3 position, glm::vec3 Up = glm::vec3(0, 1, 0), float near = 0.1f, float far = 1000.0f, float pitch = 0.0f, float yaw = 90.0f, float width = 800, float height = 600, float fov = 45.0);

    // 尝试了一种基于屏幕中心坐标的视角变换
    void processMouseMovement1(float xPos, float yPos);

    // 基于偏移的视角变换
    void processMouseMovement2(float xoffset, float yoffset);

    void processMouseScroll(float yOffset);

    void processKeyboard(int mode, float deltaTime);

    glm::mat4 getViewMatrix()
    {
        updateView();
        return viewMatrix; 
    }

    glm::mat4 getProjectionMatrix()
    {
        updateProjection();
        return projectionMatrix;
    }

    void updateWindow(float width, float height)
    {
        screenWidth = width;
        screenHeight = height;
    }

    void setPosition(glm::vec3 Position)
    {
        cameraPos = Position;
    }

private:
    void updateView();

    void updateProjection()
    {
        projectionMatrix = glm::mat4(1.0f);
        projectionMatrix = glm::perspective(glm::radians(visualField), (float)screenWidth / (float)screenHeight, zNear, zFar);
    }

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    float screenWidth; // 屏幕宽度
    float screenHeight; // 屏幕高度
    float Yaw; // 偏航角，即以y轴为中心的旋转角
    float Pitch; // 俯仰角，即以x轴为中心的旋转角
    float visualField; // 投影矩阵视野
    float zNear; // 近平面
    float zFar; // 远平面
};

Camera::Camera(glm::vec3 position, glm::vec3 Up, float width, float height, float fov, float near, float far)
{
    cameraPos = position;
    cameraUp = Up;

    screenHeight = height;
    screenWidth = width;
    visualField = fov;
    zNear = near;
    zFar = far;
    Yaw = -90.0f;
    Pitch = 0.0f;

    // 更新投影矩阵和视图矩阵
    updateProjection();
    updateView();
}

void Camera::InitCamera(glm::vec3 position, glm::vec3 Up, float near, float far, float pitch, float yaw, float width, float height, float fov)
{
    cameraPos = position;
    cameraUp = Up;

    screenHeight = height;
    screenWidth = width;
    visualField = fov;
    zNear = 0.1f;
    zFar = far;
    Yaw = yaw;
    Pitch = pitch;

    // 更新投影矩阵和视图矩阵
    updateProjection();
    updateView();
}

void Camera::processMouseMovement1(float xPos, float yPos)
{
    float xoffset = xPos - screenWidth/2;
    float yoffset = screenHeight/2 - yPos;

    // std::cout << xoffset << " , " << yoffset << std::endl;

    Yaw = 90 + 90 * xoffset/(screenWidth/2);
    Pitch = 60 * yoffset/(screenHeight/2);
    // std::cout << Yaw << " " << Pitch << std::endl;

    Pitch = std::max(-89.0f, Pitch);
    Pitch = std::min(89.0f, Pitch);

    // updateView();
}

void Camera::processMouseMovement2(float xoffset, float yoffset)
{
    // float xoffset = xPos - 0.5;
    // float yoffset = 0.5 - yPos;

    // std::cout << xoffset << " , " << yoffset << std::endl;

    Yaw += (0.3f * xoffset);
    Pitch += (0.3f * yoffset);
    // std::cout << xoffset << " " << yoffset << std::endl;

    Pitch = std::max(-89.0f, Pitch);
    Pitch = std::min(89.0f, Pitch);
    Yaw = std::max(0.0f, Yaw);
    Yaw = std::min(180.0f, Yaw);

    // updateView();
}

void Camera::processMouseScroll(float yOffset)
{
    visualField -= yOffset;
    visualField = std::max(1.0f, visualField);
    visualField = std::min(60.0f, visualField);
    // updateProjection();
}

void Camera::processKeyboard(int mode, float deltaTime)
{   
    switch(mode)
    {
        case(FORWARD):{cameraPos += (deltaTime*10*cameraFront); break;}
        case(BACKWARD):{cameraPos -= (deltaTime*10*cameraFront); break;}
        case(LEFT):{cameraPos -= (deltaTime*10*cameraRight); break;}
        case(RIGHT):{cameraPos += (deltaTime*10*cameraRight); break;}
    }
    // updateView();
}

void Camera::updateView()
{
    cameraFront.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
    cameraFront.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
    cameraFront.y = sin(glm::radians(Pitch));
    cameraFront = glm::normalize(cameraFront);
    cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
}

#endif