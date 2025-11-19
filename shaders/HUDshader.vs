#version 450 core
layout (location = 0) in vec2 aPos;    // 屏幕坐标（像素）
layout (location = 1) in vec2 aTexCoord;  // 纹理坐标

out vec2 TexCoords;  // 传递给片元着色器的纹理坐标

uniform mat4 orthoMatrix;  // 正交投影矩阵（由CPU传入）

void main() {
    // 正交矩阵转换屏幕坐标到NDC
    gl_Position = orthoMatrix * vec4(aPos, 0.0f, 1.0f);
    TexCoords = aTexCoord;  // 传递纹理坐标
}