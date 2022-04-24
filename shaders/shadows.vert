#version 330 core


layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 uvs;
layout (location = 3) in ivec4 bones;
layout (location = 4) in vec4 weights;


const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 finalBonesMatrices[MAX_BONES];
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

uniform bool hasAnimation;

void main()
{
    // gl_Position = lightSpaceMatrix * model * vec4(positions, 1.0);

    if(!hasAnimation) {
        gl_Position = lightSpaceMatrix * model * vec4(positions, 1.0);
    }

    else {
        vec4 totalPosition = vec4(0.0f);
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(bones[i] == -1) 
                continue;

            if(bones[i] >= MAX_BONES) 
            {
                totalPosition = vec4(positions,1.0f);
                break;
            }
        
            vec4 localPosition = finalBonesMatrices[bones[i]] * vec4(positions,1.0f);
            totalPosition += localPosition * weights[i];
        }

        gl_Position = lightSpaceMatrix * model * vec4(totalPosition);
    }
} 