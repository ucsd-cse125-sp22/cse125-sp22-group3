#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 uvs;
layout (location = 3) in ivec4 boneIds; 
layout (location = 4) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform bool hasAnimation;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform mat4 finalBonesMatrices[MAX_BONES];

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out float sampleExtraOutput;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

void main()
{
    if(hasAnimation) {
        vec4 totalPosition = vec4(0.0f);
        vec3 totalNormal = vec3(0.0f);

        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(boneIds[i] == -1) 
                continue;

            if(boneIds[i] >= 100) 
            {
                totalPosition = vec4(positions,1.0f);
                totalNormal = normals;
                break;
            }
        
        
            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(positions,1.0f);
            totalPosition += localPosition * weights[i];
            vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * normals;
            totalNormal += localNormal * vec3(weights[i]);
        }
    
        FragPos = vec3(model * totalPosition);
        Normal = mat3(transpose(inverse(model))) * totalNormal;  
        TexCoords = uvs;

        FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    
        gl_Position = projection * view * model * totalPosition;
    }

    else {
       FragPos = vec3(model * vec4(positions, 1.0));
       Normal = mat3(transpose(inverse(model))) * normals;  
       TexCoords = uvs;

       FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    
       gl_Position = projection * view * vec4(FragPos, 1.0);
    }
}