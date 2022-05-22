#version 330 core
#define PI 3.1415926538

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float time;
uniform sampler2D texture_normal1;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat4 viewMat;
out float t;

void main()
{
    vec2 tiling_and_offset = uv + vec2(time * 0.025f, 1.0f);
    vec4 noiseSample = texture(texture_normal1, tiling_and_offset);
    vec3 offset = 2.0f * (vec3(-1.0f, 1.0f, 0.0f)) * noiseSample.rgb;
    // vec3 offset = 1.0f * noiseSample.rgb;
    vec3 newPos = offset + position;

    // Output variables
    FragPos = vec3(model * vec4(newPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;  
    TexCoords = uv;

    viewMat = view;
    t = time; 

    gl_Position = projection * view * vec4(FragPos, 1.0f);
}