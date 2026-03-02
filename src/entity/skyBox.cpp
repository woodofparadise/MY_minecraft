#include "skyBox.h"
#include <cmath>

// 通用关键帧插值：在 times[] 和 colors[] 之间按 time 做分段线性插值
static glm::vec3 lerpKeyframes(float time, const float* times, const glm::vec3* colors, int n)
{
    int i = 0;
    while (i < n - 2 && times[i + 1] <= time) i++;
    float t = (time - times[i]) / (times[i + 1] - times[i]);
    return glm::mix(colors[i], colors[i + 1], t);
}

glm::vec3 SkyBox::getHorizonColor() const
{
    static const float times[] =      { 0.0f,  0.20f, 0.25f, 0.30f, 0.50f, 0.70f, 0.75f, 0.80f, 1.0f };
    static const glm::vec3 colors[] = {
        {0.02f, 0.02f, 0.08f},  // 午夜
        {0.02f, 0.02f, 0.08f},  // 拂晓前
        {0.80f, 0.40f, 0.20f},  // 日出
        {0.60f, 0.80f, 1.00f},  // 清晨
        {0.50f, 0.80f, 1.00f},  // 正午
        {0.60f, 0.80f, 1.00f},  // 下午
        {0.80f, 0.30f, 0.10f},  // 日落
        {0.05f, 0.05f, 0.15f},  // 暮色
        {0.02f, 0.02f, 0.08f},  // 午夜（循环闭合）
    };
    return lerpKeyframes(timeOfDay, times, colors, sizeof(times) / sizeof(times[0]));
}

glm::vec3 SkyBox::getZenithColor() const
{
    static const float times[] =      { 0.0f,  0.20f, 0.25f, 0.30f, 0.50f, 0.70f, 0.75f, 0.80f, 1.0f };
    static const glm::vec3 colors[] = {
        {0.01f, 0.01f, 0.05f},  // 午夜
        {0.01f, 0.01f, 0.05f},  // 拂晓前
        {0.20f, 0.30f, 0.60f},  // 日出
        {0.40f, 0.70f, 1.00f},  // 清晨
        {0.30f, 0.60f, 1.00f},  // 正午
        {0.40f, 0.70f, 1.00f},  // 下午
        {0.30f, 0.20f, 0.50f},  // 日落
        {0.02f, 0.02f, 0.10f},  // 暮色
        {0.01f, 0.01f, 0.05f},  // 午夜（循环闭合）
    };
    return lerpKeyframes(timeOfDay, times, colors, sizeof(times) / sizeof(times[0]));
}

glm::vec3 SkyBox::getAmbientColor() const
{
    static const float times[] =      { 0.0f,  0.20f, 0.25f, 0.30f, 0.50f, 0.70f, 0.75f, 0.80f, 1.0f };
    static const glm::vec3 colors[] = {
        {0.15f, 0.15f, 0.25f},  // 午夜：冷蓝调
        {0.15f, 0.15f, 0.25f},  // 拂晓前
        {1.00f, 0.70f, 0.50f},  // 日出：暖色
        {1.00f, 1.00f, 1.00f},  // 清晨：无色偏
        {1.00f, 1.00f, 1.00f},  // 正午：无色偏
        {1.00f, 1.00f, 1.00f},  // 下午：无色偏
        {1.00f, 0.60f, 0.40f},  // 日落：暖橙色
        {0.20f, 0.20f, 0.35f},  // 暮色：转冷
        {0.15f, 0.15f, 0.25f},  // 午夜（循环闭合）
    };
    return lerpKeyframes(timeOfDay, times, colors, sizeof(times) / sizeof(times[0]));
}

void SkyBox::init()
{
    // 全屏四边形，覆盖 NDC [-1,1]×[-1,1]，两个三角形
    float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void SkyBox::update(float deltaTime)
{
    timeOfDay = fmod(timeOfDay + 10 * deltaTime * daySpeed, 1.0f);
}

void SkyBox::render(Shader& skyShader, glm::mat4 view, glm::mat4 proj, glm::vec3 cameraPos)
{
    skyShader.use();

    // 计算逆VP矩阵，用于片段着色器中反投影屏幕坐标到世界空间
    glm::mat4 invVP = glm::inverse(proj * view);
    skyShader.set_mat4("invViewProj", invVP);

    // 传入天空颜色和摄像机位置
    glm::vec3 zenith  = getZenithColor();
    glm::vec3 horizon = getHorizonColor();
    skyShader.set_vec3("skyColorZenith", zenith);
    skyShader.set_vec3("skyColorHorizon", horizon);
    skyShader.set_vec3("cameraPos", cameraPos);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}