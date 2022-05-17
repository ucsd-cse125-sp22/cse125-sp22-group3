#version 460 core

layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 uvs;
layout (location = 3) in ivec4 bones;
layout (location = 4) in vec4 weights;

out VS_OUT
{
    vec2 v_TexCoord;
} vs_out;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 finalBonesMatrices[MAX_BONES];
uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform float time;
uniform float waveSpeed;
uniform float strength;

uniform bool hasAnimation;
uniform bool isLeaf;

uniform sampler2D leaf;

void main()
{
    if(!hasAnimation) {
        vs_out.v_TexCoord = uvs;

        if(isLeaf) {
            vec4 tex = texture(leaf, uvs + vec2(time * strength));
            tex = tex * 2.0f - vec4(1.0f);
            tex *= 5.0f;

            vec3 newPos = positions;

            if(positions.y > 0.0f) { 
                newPos = positions + vec3(1.0f, 0.0f, 0.0f) * sin(time * waveSpeed);
            }

            newPos = newPos + normals * vec3(tex);

            gl_Position = model * vec4(newPos, 1.0f);
        }

        else {
            gl_Position = model * vec4(positions, 1.0);
        }
    }

    else {
        vs_out.v_TexCoord = uvs;
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

        gl_Position = model * vec4(totalPosition);
    }
} 