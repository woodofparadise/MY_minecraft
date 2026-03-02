#version 450 core

in vec2 ScreenPos;

uniform mat4 invViewProj;
uniform vec3 skyColorZenith;
uniform vec3 skyColorHorizon;
uniform vec3 cameraPos;

out vec4 FragColor;

void main()
{
    // 通过逆VP矩阵将屏幕坐标反投影回世界空间，得到视线方向
    vec4 clipPos = vec4(ScreenPos, 1.0, 1.0);
    vec4 worldPos = invViewProj * clipPos;
    vec3 viewDir = normalize(worldPos.xyz / worldPos.w - cameraPos);

    // 根据视线方向的 y 分量做天顶-地平线渐变
    float t = pow(max(viewDir.y, 0.0), 0.5);
    vec3 color = mix(skyColorHorizon, skyColorZenith, t);

    // 地平线以下：将地平线色逐渐压暗
    if (viewDir.y < 0.0)
    {
        float below = clamp(-viewDir.y * 3.0, 0.0, 1.0);
        color = mix(skyColorHorizon, skyColorHorizon * 0.5, below);
    }

    FragColor = vec4(color, 1.0);
}
