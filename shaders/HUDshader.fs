#version 450 core

in vec2 TexCoords;

uniform sampler2D blockTexture;
uniform sampler2D cursorTexture;
uniform sampler2D toolbarTexture;

uniform int textureUsed;
uniform int blockType;

out vec4 FragColor;

vec2 block_texture_map(vec2 TexCoords_temp)
{
    TexCoords_temp = TexCoords_temp / 16.0f;
    switch(blockType)
    {
        case(0):
        {
            discard;
        }
        case(1):
        {
            return TexCoords_temp+vec2(0.0f, 15.0f/16.0f);
        }
        case(2):
        {
            return TexCoords_temp+vec2(1.0f/16.0f, 15.0f/16.0f);
        }
        case(3):
        {
            return TexCoords_temp+vec2(2.0f/16.0f, 14.0f/16.0f);
        }
        case(4):
        {
            return TexCoords_temp+vec2(13.0f/16.0f, 3.0f/16.0f);
        }
        case(5):
        {
            return TexCoords_temp+vec2(2.0f/16.0f, 15.0f/16.0f);
        }
        case(6):
        {
            return TexCoords_temp+vec2(4.0f/16.0f, 14.0f/16.0f);
        }
        case(7):
        {
            return TexCoords_temp+vec2(1.0f/16.0f, 12.0f/16.0f);
        }
        case(8):
        {
            return TexCoords_temp+vec2(2.0f/16.0f, 13.0f/16.0f);
        }
        case(9):
        {
            return TexCoords_temp+vec2(1.0f/16.0f, 13.0f/16.0f);
        }
        case(10):
        {
            return TexCoords_temp+vec2(0.0f, 13.0f/16.0f);
        }
        case(11):
        {
            return TexCoords_temp+vec2(2.0f/16.0f, 12.0f/16.0f);
        }
    }
    return TexCoords_temp;
}

void main()
{
    vec4 texColor;
    
    // 根据 textureUsed 选择纹理
    switch (textureUsed)
    {
        case 1:
            vec2 TexCoord = block_texture_map(TexCoords);
            texColor = texture(blockTexture, TexCoord);
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