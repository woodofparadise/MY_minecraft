#ifndef PLAYER_H
#define PLAYER_H

#include "texture.h"
#include "Shader.h"
#include "camera.h"
#include "collision.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

vector<Vertex> create_cuboid(float x, float y, float z, glm::vec3 offset)
{
    // y是高度
    vector<Vertex> result(24);

    // 下
    result[0].Position = glm::vec3(-x/2, -y/2, -z/2);
    result[1].Position = glm::vec3(-x/2, -y/2, z/2);
    result[2].Position = glm::vec3(x/2, -y/2, z/2);
    result[3].Position = glm::vec3(x/2, -y/2, -z/2);
    for(int i = 0; i < 4; i++)
    {
        result[i].Normal = glm::vec3(0, -1, 0);
    }

    // 上
    result[4].Position = glm::vec3(-x/2, y/2, -z/2);
    result[5].Position = glm::vec3(-x/2, y/2, z/2);
    result[6].Position = glm::vec3(x/2, y/2, z/2);
    result[7].Position = glm::vec3(x/2, y/2, -z/2);
    for(int i = 4; i < 8; i++)
    {
        result[i].Normal = glm::vec3(0, 1, 0);
    }

    // 左
    result[8].Position = glm::vec3(-x/2, -y/2, -z/2);
    result[9].Position = glm::vec3(-x/2, y/2, -z/2);
    result[10].Position = glm::vec3(-x/2, y/2, z/2);
    result[11].Position = glm::vec3(-x/2, -y/2, z/2);
    for(int i = 8; i < 12; i++)
    {
        result[i].Normal = glm::vec3(-1, 0, 0);
    }

    // 右
    result[12].Position = glm::vec3(x/2, -y/2, -z/2);
    result[13].Position = glm::vec3(x/2, y/2, -z/2);
    result[14].Position = glm::vec3(x/2, y/2, z/2);
    result[15].Position = glm::vec3(x/2, -y/2, z/2);
    for(int i = 12; i < 16; i++)
    {
        result[i].Normal = glm::vec3(1, 0, 0);
    }

    // 前
    result[16].Position = glm::vec3(-x/2, -y/2, z/2);
    result[17].Position = glm::vec3(-x/2, y/2, z/2);
    result[18].Position = glm::vec3(x/2, y/2, z/2);
    result[19].Position = glm::vec3(x/2, -y/2, z/2);
    for(int i = 16; i < 20; i++)
    {
        result[i].Normal = glm::vec3(0, 0, 1);
    }

    // 后
    result[20].Position = glm::vec3(-x/2, -y/2, -z/2);
    result[21].Position = glm::vec3(-x/2, y/2, -z/2);
    result[22].Position = glm::vec3(x/2, y/2, -z/2);
    result[23].Position = glm::vec3(x/2, -y/2, -z/2);
    for(int i = 20; i < 24; i++)
    {
        result[i].Normal = glm::vec3(0, 0, -1);
    }

    for(int i = 0; i < 24; i++)
    {
        result[i].Position += offset;
    }

    return result;
}

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

const float gravity = -9.8f/10;   // 重力加速度

class Player
{
    private:
        std::vector<unsigned int> indices;
        std::vector<Vertex> vertices;
        Texture playerTexture;
        bool cameraMode = true; // 控制第三人称和第一人称切换
        glm::vec3 velocity;
        glm::vec3 playerSize = glm::vec3(0.6f, 1.8f, 0.6f);
        bool isOnGround = false;            // 是否在地面上
        bool isJumping = false;     // 是否正在跳跃
        float jumpForce = 0.1f;     // 跳跃初速度
    
    public:
        unsigned int EBO, VAO, VBO;
        Camera camera;
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);   // 玩家脚底中间的坐标
        AABB playerBox;
        Player();

        void upload_data(char const* path);
        
        void bind_player_texture(Shader& playerShader);

        void draw_player(Shader& playerShader);

        void move(int mode, float deltaTime);

        void update_position(Terrain& terrain, float& deltaTime);

        void set_position(glm::vec3 initPosition)
        {
            position = initPosition;
            playerBox.update_AABB(position+glm::vec3(0.0f, 0.9f, 0.0f), playerSize);
            camera.set_position(position+glm::vec3(0.0f, ARM_LEG_SIZE_Y+BODY_SIZE_Y+HEAD_SIZE/2, HEAD_SIZE));
        }

        void switch_mode()
        {
            cameraMode = false;
        }

        void jump();
};

Player::Player()
{   
    // 左腿
    vector<Vertex> leftLeg = create_cuboid(ARM_LEG_SIZE_X, ARM_LEG_SIZE_Y, ARM_LEG_SIZE_Z, glm::vec3(-ARM_LEG_SIZE_X/2, ARM_LEG_SIZE_Y/2, 0));
    for(int i = 0; i < 24; i++)
    {
        leftLeg[i].Texcoord = textureOffset[i] + glm::vec2(16.0f/64, 0.0f);
    }
    for(int i = 0; i < 24; i+=4)
    {
        vertices.push_back(leftLeg[i]);
        vertices.push_back(leftLeg[i+1]);
        vertices.push_back(leftLeg[i+2]);
        vertices.push_back(leftLeg[i+3]);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-3);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-1);
    }

    // 右腿
    vector<Vertex> rightLeg = create_cuboid(ARM_LEG_SIZE_X, ARM_LEG_SIZE_Y, ARM_LEG_SIZE_Z, glm::vec3(ARM_LEG_SIZE_X/2, ARM_LEG_SIZE_Y/2, 0));
    for(int i = 0; i < 24; i++)
    {
        rightLeg[i].Texcoord = textureOffset[i] + glm::vec2(0.0f, 32.0f/64.0f);
    }
    for(int i = 0; i < 24; i+=4)
    {
        vertices.push_back(rightLeg[i]);
        vertices.push_back(rightLeg[i+1]);
        vertices.push_back(rightLeg[i+2]);
        vertices.push_back(rightLeg[i+3]);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-3);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-1);
    }

    // 身体
    vector<Vertex> body = create_cuboid(BODY_SIZE_X, BODY_SIZE_Y, BODY_SIZE_Z, glm::vec3(0, ARM_LEG_SIZE_Y+BODY_SIZE_Y/2, 0));
    body[0].Texcoord = glm::vec2(28.0f/64.0f, 48.0f/64.0f);
    body[1].Texcoord = glm::vec2(36.0f/64.0f, 48.0f/64.0f);
    body[2].Texcoord = glm::vec2(36.0f/64.0f, 44.0f/64.0f);
    body[3].Texcoord = glm::vec2(28.0f/64.0f, 44.0f/64.0f);//
    body[4].Texcoord = glm::vec2(20.0f/64.0f, 48.0f/64.0f);
    body[5].Texcoord = glm::vec2(28.0f/64.0f, 48.0f/64.0f);
    body[6].Texcoord = glm::vec2(28.0f/64.0f, 44.0f/64.0f);
    body[7].Texcoord = glm::vec2(20.0f/64.0f, 44.0f/64.0f);//
    body[8].Texcoord = glm::vec2(16.0f/64.0f, 32.0f/64.0f);
    body[9].Texcoord = glm::vec2(16.0f/64.0f, 44.0f/64.0f);
    body[10].Texcoord = glm::vec2(20.0f/64.0f, 44.0f/64.0f);
    body[11].Texcoord = glm::vec2(20.0f/64.0f, 32.0f/64.0f);//
    body[12].Texcoord = glm::vec2(32.0f/64.0f, 32.0f/64.0f);
    body[13].Texcoord = glm::vec2(32.0f/64.0f, 44.0f/64.0f);
    body[14].Texcoord = glm::vec2(28.0f/64.0f, 44.0f/64.0f);
    body[15].Texcoord = glm::vec2(28.0f/64.0f, 32.0f/64.0f);//
    body[16].Texcoord = glm::vec2(20.0f/64.0f, 32.0f/64.0f);
    body[17].Texcoord = glm::vec2(20.0f/64.0f, 44.0f/64.0f);
    body[18].Texcoord = glm::vec2(28.0f/64.0f, 44.0f/64.0f);
    body[19].Texcoord = glm::vec2(28.0f/64.0f, 32.0f/64.0f);//
    body[20].Texcoord = glm::vec2(32.0f/64.0f, 32.0f/64.0f);
    body[21].Texcoord = glm::vec2(32.0f/64.0f, 44.0f/64.0f);
    body[22].Texcoord = glm::vec2(40.0f/64.0f, 44.0f/64.0f);
    body[23].Texcoord = glm::vec2(40.0f/64.0f, 32.0f/64.0f);//
    for(int i = 0; i < 24; i+=4)
    {
        vertices.push_back(body[i]);
        vertices.push_back(body[i+1]);
        vertices.push_back(body[i+2]);
        vertices.push_back(body[i+3]);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-3);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-1);
    }

    // 左手
    vector<Vertex> leftArm = create_cuboid(ARM_LEG_SIZE_X, ARM_LEG_SIZE_Y, ARM_LEG_SIZE_Z, glm::vec3(-BODY_SIZE_X/2-ARM_LEG_SIZE_X/2, ARM_LEG_SIZE_Y+BODY_SIZE_Y-ARM_LEG_SIZE_Y/2, 0));
    for(int i = 0; i < 24; i++)
    {
        leftArm[i].Texcoord = textureOffset[i] + glm::vec2(32.0f/64.0f, 0.0f);
    }
    for(int i = 0; i < 24; i+=4)
    {
        vertices.push_back(leftArm[i]);
        vertices.push_back(leftArm[i+1]);
        vertices.push_back(leftArm[i+2]);
        vertices.push_back(leftArm[i+3]);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-3);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-1);
    }

    // 右手
    vector<Vertex> rightArm = create_cuboid(ARM_LEG_SIZE_X, ARM_LEG_SIZE_Y, ARM_LEG_SIZE_Z, glm::vec3(BODY_SIZE_X/2+ARM_LEG_SIZE_X/2, ARM_LEG_SIZE_Y+BODY_SIZE_Y-ARM_LEG_SIZE_Y/2, 0));
    for(int i = 0; i < 24; i++)
    {
        rightArm[i].Texcoord = textureOffset[i] + glm::vec2(40.0f/64.0f, 32.0f/64.0f);
    }
    for(int i = 0; i < 24; i+=4)
    {
        vertices.push_back(rightArm[i]);
        vertices.push_back(rightArm[i+1]);
        vertices.push_back(rightArm[i+2]);
        vertices.push_back(rightArm[i+3]);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-3);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-1);
    }

    // 头部
    vector<Vertex> head = create_cuboid(HEAD_SIZE, HEAD_SIZE, HEAD_SIZE, glm::vec3(0, ARM_LEG_SIZE_Y+BODY_SIZE_Y+HEAD_SIZE/2, 0));
    head[0].Texcoord = glm::vec2(16.0f/64.0f, 1.0f);
    head[1].Texcoord = glm::vec2(24.0f/64.0f, 1.0f);
    head[2].Texcoord = glm::vec2(24.0f/64.0f, 56.0f/64.0f);
    head[3].Texcoord = glm::vec2(16.0f/64.0f, 56.0f/64.0f);//
    head[4].Texcoord = glm::vec2(8.0f/64.0f, 1.0f);
    head[5].Texcoord = glm::vec2(16.0f/64.0f, 1.0f);
    head[6].Texcoord = glm::vec2(16.0f/64.0f, 56.0f/64.0f);
    head[7].Texcoord = glm::vec2(8.0f/64.0f, 56.0f/64.0f);//
    head[8].Texcoord = glm::vec2(0.0f, 48.0f/64.0f);
    head[9].Texcoord = glm::vec2(0.0f, 56.0f/64.0f);
    head[10].Texcoord = glm::vec2(8.0f/64.0f, 56.0f/64.0f);
    head[11].Texcoord = glm::vec2(8.0f/64.0f, 48.0f/64.0f);//
    head[12].Texcoord = glm::vec2(24.0f/64.0f, 48.0f/64.0f);
    head[13].Texcoord = glm::vec2(24.0f/64.0f, 56.0f/64.0f);
    head[14].Texcoord = glm::vec2(16.0f/64.0f, 56.0f/64.0f);
    head[15].Texcoord = glm::vec2(16.0f/64.0f, 48.0f/64.0f);//
    head[16].Texcoord = glm::vec2(8.0f/64.0f, 48.0f/64.0f);
    head[17].Texcoord = glm::vec2(8.0f/64.0f, 56.0f/64.0f);
    head[18].Texcoord = glm::vec2(16.0f/64.0f, 56.0f/64.0f);
    head[19].Texcoord = glm::vec2(16.0f/64.0f, 48.0f/64.0f);//
    head[20].Texcoord = glm::vec2(24.0f/64.0f, 48.0f/64.0f);
    head[21].Texcoord = glm::vec2(24.0f/64.0f, 56.0f/64.0f);
    head[22].Texcoord = glm::vec2(32.0f/64.0f, 56.0f/64.0f);
    head[23].Texcoord = glm::vec2(32.0f/64.0f, 48.0f/64.0f);//
    for(int i = 0; i < 24; i+=4)
    {
        vertices.push_back(head[i]);
        vertices.push_back(head[i+1]);
        vertices.push_back(head[i+2]);
        vertices.push_back(head[i+3]);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-3);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-4);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-1);
    }

    vertices.shrink_to_fit();
    indices.shrink_to_fit();
}

void Player::upload_data(char const* path)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (size_t)(vertices.size() * sizeof(Vertex)), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Texcoord));
    glEnableVertexAttribArray(0);
    
    playerTexture.load_texture(path);
    return ;
}

void Player::bind_player_texture(Shader& playerShader)
{
    playerShader.use();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, playerTexture.TextureID);
    glActiveTexture(GL_TEXTURE0);
}

void Player::draw_player(Shader& playerShader)
{
    playerShader.use();
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, playerTexture.TextureID);
    playerShader.set_int("textureUsed", 1);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    playerShader.set_mat4("model", model);
    // model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // model = glm::scale(model, glm::vec3(0.1f));
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Player::move(int mode, float deltaTime)
{   
    switch(mode)
    {
        case(FORWARD):{velocity += (deltaTime*10*glm::vec3(camera.cameraFront.x, 0.0f, camera.cameraFront.z)); break;}
        case(BACKWARD):{velocity -= (deltaTime*10*glm::vec3(camera.cameraFront.x, 0.0f, camera.cameraFront.z)); break;}
        case(LEFT):{velocity -= (deltaTime*10*glm::vec3(camera.cameraRight.x, 0.0f, camera.cameraRight.z)); break;}
        case(RIGHT):{velocity += (deltaTime*10*glm::vec3(camera.cameraRight.x, 0.0f, camera.cameraRight.z)); break;}
    }
}

void Player::jump() 
{
    // 如果在地面并且不在跳跃中那么正常起跳
    if (isOnGround && !isJumping) 
    {
        velocity.y = jumpForce;
        isJumping = true;
        isOnGround = false;
    }
}

void Player::update_position(Terrain& terrain, float& deltaTime)
{
    // 根据速度更新位置
    resolve_collisions(position, playerSize, velocity, playerBox, terrain);
    // 更新碰撞箱
    playerBox.update_AABB(position+glm::vec3(0.0f, 0.9f, 0.0f), playerSize);
    isOnGround = (terrain.get_block_type(position - glm::vec3(0.0f, 0.1f, 0.0f)) != AIR);
    // 在地面并且速度向下的时候重置状态
    if (isOnGround && velocity.y <= 0) 
    {
        isJumping = false;
        velocity.y = 0; // 防止在地面上积累重力
    }
    // 不在地面的时候需要考虑重力加速度
    else if(!isOnGround)
    {
        velocity.y += gravity*deltaTime;
    }
    // 其他两个轴的速度每一帧重置
    velocity.x = 0.0f;
    velocity.z = 0.0f;
    // 更新相机位置
    if(cameraMode)
    {
        camera.set_position(position+glm::vec3(0.0f, ARM_LEG_SIZE_Y+BODY_SIZE_Y+HEAD_SIZE/2, HEAD_SIZE));
    }
    else
    {
        camera.set_position(position+glm::vec3(0.0f, (ARM_LEG_SIZE_Y+BODY_SIZE_Y+HEAD_SIZE/2) * 1.5, -8 * HEAD_SIZE));
    }
}

#endif