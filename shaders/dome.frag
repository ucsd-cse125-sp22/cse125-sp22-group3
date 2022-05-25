#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D texture_diffuse1;

// You can output many things. The first vec4 type output determines the color of the fragment

void main()
{
    vec4 tex =  texture(texture_diffuse1, texCoords);

    FragColor = tex;
}