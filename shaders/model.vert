#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 uvs;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out float sampleExtraOutput;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main()
{
    FragPos = vec3(model * vec4(positions, 1.0));
    Normal = mat3(transpose(inverse(model))) * normals;  
    TexCoords = uvs;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}