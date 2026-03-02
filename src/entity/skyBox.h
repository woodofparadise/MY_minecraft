#ifndef SKYBOX_H
#define SKYBOX_H

#include "../render/Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class SkyBox
{
private:
    float timeOfDay = 0.0f;     // 初始为上午
    float daySpeed = 1.0f / 600.0f;  // 日夜循环速度
    unsigned int quadVAO, quadVBO;
public:
    void init();
    void update(float deltaTime);
    void render(Shader& skyShader, glm::mat4 view, glm::mat4 proj, glm::vec3 cameraPos);
    glm::vec3 getHorizonColor() const;
    glm::vec3 getZenithColor() const;
    void clear()
    {
        if (quadVAO != 0) glDeleteVertexArrays(1, &quadVAO);
        if (quadVBO != 0) glDeleteBuffers(1, &quadVBO);
    }
};

#endif