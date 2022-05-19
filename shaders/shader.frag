#version 330

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{
    fragColor = vec4(vec3(1.0f), 1.0f);
    brightColor = vec4(vec3(0.0f), 1.0f);
}