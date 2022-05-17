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
uniform float time;
uniform float waveSpeed;
uniform float strength;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out mat4 viewMat;

uniform sampler2D texture_height1;

void main()
{
    FragPos = vec3(model * vec4(positions, 1.0));
    Normal = mat3(transpose(inverse(model))) * normals;  
    vec4 tex = texture(texture_height1, uvs + vec2(time * strength, time * strength));
    tex = tex * 2.0f - vec4(1.0f);
    tex *= 2.5f;

    vec3 newPos = positions;

    if(positions.y > 0.0f) { 
        newPos = positions + vec3(0.5f, 0.0f, 0.0f) * sin(time * waveSpeed);
    }

    newPos = newPos + normals * vec3(tex);
    TexCoords = uvs;

    viewMat = view;
    
    gl_Position = projection * view * model * vec4(newPos, 1.0);
}