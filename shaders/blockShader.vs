#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aLightLevel;  // 编码光照：sky + block/16

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

out vec2 TexCoords;
out float SkyLight;    // 解码后的天空光 0~15
out float BlockLight;  // 解码后的方块光 0~15
out float FragDist;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    TexCoords = aTexCoords;

    // 在顶点着色器中解码，避免片段插值后 floor/fract 精度问题
    // +0.001 防止 14.99999 被 floor 到 14
    SkyLight   = floor(aLightLevel + 0.001);
    BlockLight = fract(aLightLevel + 0.001) * 16.0;

    FragDist = length(viewPos - worldPos.xyz);
}
