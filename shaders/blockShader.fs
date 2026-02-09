#version 450 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D blockTexture;
uniform sampler2D playerTexture;

uniform int textureUsed;

out vec4 FragColor;



void main()
{
    // FragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    switch(textureUsed)
    {
        case(0):{FragColor = texture(blockTexture, TexCoords); break;}
        case(1):{FragColor = texture(playerTexture, TexCoords); break;}
        default:{FragColor = vec4(0.5f);}
    }

    return ;
}