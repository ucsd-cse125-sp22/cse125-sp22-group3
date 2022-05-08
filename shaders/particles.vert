#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 uvs;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoords;

void main()
{
    texCoords = uvs;
    gl_Position = projection * view * model * vec4(position, 1.0);
}