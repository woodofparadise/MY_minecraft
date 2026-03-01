#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aLightLevel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

out vec2 TexCoords;
out float LightLevel;
out float FragDist;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    TexCoords = aTexCoords;
    LightLevel = aLightLevel;
    FragDist = length(viewPos - worldPos.xyz);
}
