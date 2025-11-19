#version 450 core

in vec2 TexCoords;

uniform sampler2D cursorTexture;

out vec4 FragColor;

void main()
{
    float alpha = texture(cursorTexture, TexCoords).a;
    if (alpha < 0.1) 
    {
        discard;
    }
    FragColor = texture(cursorTexture, TexCoords);
    return ;
}