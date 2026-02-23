#ifndef PLAYER_H
#define PLAYER_H

#include "../render/texture.h"
#include "../render/Shader.h"
#include "../core/camera.h"
#include "collision.h"
#include "../ui/toolBar.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

vector<Vertex> create_cuboid(float x, float y, float z, glm::vec3 offset);

// 人物模型各部位尺寸
const float HEAD_SIZE = 0.5f;
const float BODY_SIZE_X = 0.5f, BODY_SIZE_Y = 0.75f, BODY_SIZE_Z = 0.25f;
const float ARM_LEG_SIZE_X = 0.25f, ARM_LEG_SIZE_Y = 0.75f, ARM_LEG_SIZE_Z = 0.25f;

// 纹理坐标的相对偏移
const glm::vec2 textureOffset[24] =
{
    glm::vec2(8.0f/64.0f, 16.0f/64.0f), glm::vec2(12.0f/64.0f, 16.0f/64.0f), glm::vec2(12.0f/64.0f, 12.0f/64.0f), glm::vec2(8.0f/64.0f, 12.0f/64.0f),
    glm::vec2(4.0f/64.0f, 16.0f/64.0f), glm::vec2(8.0f/64.0f, 16.0f/64.0f), glm::vec2(8.0f/64.0f, 12.0f/64.0f), glm::vec2(4.0f/64.0f, 12.0f/64.0f),
    glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 12.0f/64.0f), glm::vec2(4.0f/64.0f, 12.0f/64.0f), glm::vec2(4.0f/64.0f, 0.0f),
    glm::vec2(12.0f/64.0f, 0.0f), glm::vec2(12.0f/64.0f, 12.0f/64.0f), glm::vec2(8.0f/64.0f, 12.0f/64.0f), glm::vec2(8.0f/64.0f, 0.0f),
    glm::vec2(4.0f/64.0f, 0.0f/64.0f), glm::vec2(4.0f/64.0f, 12.0f/64.0f), glm::vec2(8.0f/64.0f, 12.0f/64.0f), glm::vec2(8.0f/64.0f, 0.0f),
    glm::vec2(12.0f/64.0f, 0.0f/64.0f), glm::vec2(12.0f/64.0f, 12.0f/64.0f), glm::vec2(16.0f/64.0f, 12.0f/64.0f), glm::vec2(16.0f/64.0f, 0.0f)
};

const float gravity = -40.0f;   // 重力加速度 (单位/秒²)

class Player
{
    private:
        std::vector<unsigned int> indices;
        std::vector<Vertex> vertices;
        Texture playerTexture;
        glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 playerSize = glm::vec3(0.6f, 1.8f, 0.6f);
        bool isOnGround = false;            // 是否在地面上
        bool isJumping = false;     // 是否正在跳跃
        float jumpForce = 10.0f;     // 跳跃初速度 (单位/秒)

    public:
        unsigned int EBO, VAO, VBO;
        bool cameraMode = true; // 控制第三人称和第一人称切换
        Camera camera;
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);   // 玩家脚底中间的坐标
        AABB playerBox;
        Player();

        void upload_data(char const* path);

        void bind_player_texture(Shader& playerShader);

        void draw_player(Shader& playerShader);

        void move(const MOVE_MODE& mode);

        void update_position(Terrain& terrain, const float& deltaTime);

        void set_position(glm::vec3 initPosition)
        {
            position = initPosition;
            playerBox.update_AABB(position+glm::vec3(0.0f, 0.9f, 0.0f), playerSize);
            camera.set_position(position+glm::vec3(0.0f, ARM_LEG_SIZE_Y+BODY_SIZE_Y+HEAD_SIZE/2, HEAD_SIZE));
        }

        void switch_camera_mode()
        {
            cameraMode = !cameraMode;
        }

        void jump();

        void clear()
        {
            playerTexture.clear();
            if (VAO != 0) glDeleteVertexArrays(1, &VAO);
            if (VBO != 0) glDeleteBuffers(1, &VBO);
            if (EBO != 0) glDeleteBuffers(1, &EBO);
        }
};

#endif
