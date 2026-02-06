#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 Up, float width, float height, float fov, float near, float far)
{
    cameraPos = position;
    cameraUp = Up;

    screenHeight = height;
    screenWidth = width;
    visualField = fov;
    zNear = near;
    zFar = far;
    Yaw = 90.0f;
    Pitch = 0.0f;

    // 更新投影矩阵和视图矩阵
    update_projection();
    update_view();
}

void Camera::init_camera(glm::vec3 position, glm::vec3 Up, float near, float far, float pitch, float yaw, float width, float height, float fov)
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
    update_projection();
    update_view();
}

void Camera::process_mouse_movement_1(float xPos, float yPos)
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

void Camera::process_mouse_movement_2(float xoffset, float yoffset)
{
    // float xoffset = xPos - 0.5;
    // float yoffset = 0.5 - yPos;

    // std::cout << xoffset << " , " << yoffset << std::endl;

    Yaw += (0.3f * xoffset);
    Pitch += (0.3f * yoffset);
    // std::cout << xoffset << " " << yoffset << std::endl;

    Pitch = std::max(-89.0f, Pitch);
    Pitch = std::min(89.0f, Pitch);
    // Yaw = std::max(0.0f, Yaw);
    // Yaw = std::min(180.0f, Yaw);

    // updateView();
}

void Camera::process_mouse_scroll(float yOffset)
{
    visualField -= yOffset;
    visualField = std::max(1.0f, visualField);
    visualField = std::min(60.0f, visualField);
    // updateProjection();
}

void Camera::process_keyboard(int mode, float deltaTime)
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

void Camera::update_view()
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
