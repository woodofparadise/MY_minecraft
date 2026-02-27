#version 450 core

in vec2 TexCoords;

uniform sampler2D blockTexture;
uniform sampler2D cursorTexture;
uniform sampler2D toolbarTexture;

uniform int textureUsed;

out vec4 FragColor;

void main()
{
    vec4 texColor;

    // 根据 textureUsed 选择纹理
    switch (textureUsed)
    {
        case 1:
            texColor = texture(blockTexture, TexCoords);
            break;
        case 3:
            texColor = texture(cursorTexture, TexCoords);
            break;
        case 4:
            texColor = texture(toolbarTexture, TexCoords);
            break;
        default:
            texColor = texture(cursorTexture, TexCoords);
            break;
    }

    // Alpha 测试
    if (texColor.a < 0.1)
    {
        discard;
    }

    FragColor = texColor;
}
