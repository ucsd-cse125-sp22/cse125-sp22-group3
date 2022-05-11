#version 460 core
// This is a sample fragment shader.
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

// You can output many things. The first vec4 type output determines the color of the fragment
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in mat4 viewMat;

uniform mat4 view;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2DArray shadowMap;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};
uniform float cascadePlaneDistances[16];

uniform float time;

const vec3 day = vec3(0.6, 0.5, 0.4);
// const vec3 night = vec3(0.066, 0.098, 0.415);
const vec3 night = vec3(0.066, 0.098, 0.415);

out vec4 fragColor;

void main()
{
    // Toon
    vec4 tex = texture(texture_diffuse1, TexCoords);
    // vec4 tex = vec4(1.0f);

    if(tex.a < 0.8) {
        discard;
    }

    // Directional Light
    vec3 lightPos = vec3(-2.0f, 2.0f, -1.0f);
    vec3 lightDir = normalize(lightPos);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfVector = normalize(lightPos + viewDir);

    // select cascade layer
    vec4 fragPosViewSpace = view * vec4(FragPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < 4; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = 4;
    }

    // shadows
    vec4 FragPosLightSpace = lightSpaceMatrices[layer] * vec4(FragPos, 1.0);
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;  
    float currentDepth = projCoords.z;  
    float shadow = 0.0f;

    if(currentDepth > 1.0) {
        shadow = 0.0;
    }

    else {
        float bias =  max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);  
        const float biasModifier = 0.5f;

        if (layer == 4)
        {
            bias *= 1 / (1000.0f * biasModifier);
        }

        else
        {
            bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
        }
        vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
        int halfkernelWidth = 1;
        for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
        {
	        for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
	        {
		        float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
		        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	        }
        }

        shadow /= ((halfkernelWidth*2+1)*(halfkernelWidth*2+1));
    }

    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);   

    // combine results 

    // float interpolate = clamp((time - 26.0f) / 15.0f, 0.0f, 1.0f);
    // vec3 lightColor = mix(day, night, interpolate);
    vec3 ambient = day * vec3(tex);
    vec3 diffuse = vec3(0.6f, 0.6f, 0.6f) * diff * vec3(tex);
    vec3 specular = vec3(0.1f, 0.1f, 0.1f) * spec * vec3(tex);
    
    vec4 result = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0f);
    float brightness = dot(vec3(result), vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0) {
        BrightColor = result;
    }
    else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    // check for transparency;
    // fragColor = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0f);
    FragColor = result;
}