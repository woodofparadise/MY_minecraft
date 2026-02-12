#version 450 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in float LightLevel;

uniform sampler2D blockTexture;
uniform sampler2D playerTexture;

uniform int textureUsed;

out vec4 FragColor;

void main()
{
    vec4 texColor;
    switch(textureUsed)
    {
        case(0):{texColor = texture(blockTexture, TexCoords); break;}
        case(1):{texColor = texture(playerTexture, TexCoords); break;}
        default:{texColor = vec4(0.5f);}
    }

    if (texColor.a < 0.1)
        discard;

    // 非线性亮度曲线：等级15=1.0, 等级0≈0.035
    float brightness = pow(0.8, 15.0 - LightLevel);

    FragColor = vec4(texColor.rgb * brightness, texColor.a);
}
