#version 330

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{
    fragColor = vec4(vec3(232.0f / 255.0f, 130.0f / 255.0f, 183.0f / 255.0f), 1.0f);
    brightColor = vec4(vec3(0.0f), 1.0f);
}