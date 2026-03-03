#version 450 core

in vec2 TexCoords;
in float SkyLight;     // 顶点着色器已解码的天空光 0~15
in float BlockLight;   // 顶点着色器已解码的方块光 0~15
in float FragDist;

uniform sampler2D blockTexture;
uniform sampler2D playerTexture;

uniform int textureUsed;
uniform vec2 viewRange;
uniform vec3 skyColor;
uniform vec3 ambientColor;

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

    // 天空光受 ambientColor 色调影响，方块光（火把）不受影响
    // SkyLight/BlockLight 已在顶点着色器中解码，避免插值后 floor/fract 精度问题
    float skyBrightness   = pow(0.8, 15.0 - SkyLight);
    float blockBrightness = pow(0.8, 15.0 - BlockLight);
    vec3 litColor = texColor.rgb * max(skyBrightness * ambientColor, vec3(blockBrightness));

    // 雾化
    float fogFactor = smoothstep(viewRange.x, viewRange.y, FragDist);
    vec3 finalColor = mix(litColor, skyColor, fogFactor);

    FragColor = vec4(finalColor, texColor.a);
}
