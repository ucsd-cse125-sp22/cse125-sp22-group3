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

uniform vec3 playerLight[6];
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform float lightSize;
uniform sampler2D texture_diffuse1;
uniform sampler2DArray shadowMap;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};
uniform float cascadePlaneDistances[16];

uniform float time;

const vec4 day = vec4(0.8, 0.7, 0.6, 1.0);
const vec4 night = vec4(0.0, 0.082, 0.341, 1.0);
const vec4 sunset = vec4(0.956, 0.552, 0.262, 1.0);
const vec4 sunrise = vec4(0.941, 0.415, 0.549, 1.0f);

out vec4 fragColor;

vec3 CalcPointLight(vec3 lightPos, vec3 viewDir, vec3 norm, vec3 lightColor, vec3 tex) {
    vec3 lightDir = normalize(lightPos - FragPos);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 5.0f);

    // lemme try a spotlight hehehehe
    float outerCutOff = cos(radians(90.0f));
    float cutOff = cos(radians(60.0f));
    float theta = dot(lightDir, normalize(-(vec3(0.0f, -1.0f, 0.0f)))); 
    float epsilon = (cutOff - outerCutOff);
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // attenuation
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0f + 0.09f * distance + 
  			     0.1f * (distance * distance));    

    // combine results
    vec3 ambient2  = lightColor  * vec3(tex);
    vec3 diffuse2  = lightColor * diff * vec3(tex);
    vec3 specular2 = vec3(0.5f) * spec * vec3(tex);

    diffuse2 *= intensity;
    specular2 *= intensity;
    ambient2  *= attenuation;
    diffuse2  *= attenuation;
    specular2 *= attenuation;

    return vec3(ambient2 + diffuse2 + specular2) * 2.0f;

    /*
    // combine results
    if(theta > cutOff) {
        vec3 ambient2  = lightColor  * vec3(tex);
        vec3 diffuse2  = lightColor * diff * vec3(tex);
        vec3 specular2 = vec3(0.5f) * spec * vec3(tex);

        diffuse2 *= intensity;
        specular2 *= intensity;
        ambient2  *= attenuation;
        diffuse2  *= attenuation;
        specular2 *= attenuation;

        return vec3(ambient2 + diffuse2 + specular2);
    }

    else {
        return vec3(0.0f);
    }
    */
}

void main()
{
    // Toon
    vec4 tex = texture(texture_diffuse1, TexCoords);
    // vec4 tex = vec4(1.0f);

    if(tex.a < 0.8) {
        discard;
    }

    // Directional Light
    vec3 lightDir = normalize(lightPos);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfVector = normalize(lightPos + viewDir);

    // select cascade layer
    vec4 fragPosViewSpace = viewMat * vec4(FragPos, 1.0);
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
    float NdotL = dot(norm, lightPos);
    float lightIntensity = smoothstep(0.0, 0.01, NdotL * (1 - shadow));
    // float lightIntensity = smoothstep(0.0f, 0.01f, NdotL);
    vec4 light = lightIntensity * vec4(0.5f);

    // specular
    float NdotH = dot(norm, halfVector);
    float specularIntensity = pow(NdotH * lightIntensity, 32);
    float specularIntensitySmooth = smoothstep(0.005, 0.01, specularIntensity);
    vec4 specular = specularIntensitySmooth * vec4(0.1f, 0.1f, 0.1f, 1.0f);

    float rimDot = 1 - dot(viewDir, norm);
    float rimIntensity = rimDot * pow(NdotL, 0.1);
    rimIntensity = smoothstep(0.716 - 0.01, 0.716 + 0.01, rimIntensity);
    vec4 rim = rimIntensity * vec4(1.0);

    float secondsToMinutes =  time / 60.0f;
    // float secondsToMinutes =  time;
    float convTime = clamp(secondsToMinutes, 0.0f, 15.0f);
    vec4 lightColor = day;

    vec4 base = vec4(vec3(tex * vec4(0.5f) * (tex + light + specular + rim)), 1.0f);
    vec4 result = vec4(0.0f);

    if(convTime >= 2.5f && convTime < 3.0f) {
        float interpolate = clamp((convTime - 2.5f) / 0.5f, 0.0f, 1.0f);
        lightColor = mix(day, sunset, interpolate);
    }

    else if(convTime >= 3.0f && convTime < 3.5f) {
        float interpolate = clamp((convTime - 3.0f) / 0.5f, 0.0f, 1.0f);
        lightColor = mix(sunset, night, interpolate);

        for(int i = 0; i < lightSize; i++) {
        if(playerLight[i].y == 0.0f) { break; }
            result += 
                (vec4(CalcPointLight(playerLight[i], viewDir, norm, vec3(1, 0.894, 0.419), vec3(base)), 0.0f) * interpolate);
        }
    }

    else if(convTime >= 3.5f && convTime < 8.5f) {
        lightColor = night;

        for(int i = 0; i < lightSize; i++) {
        if(playerLight[i].y == 0.0f) { break; }
            result += 
                vec4(CalcPointLight(playerLight[i], viewDir, norm, vec3(1, 0.894, 0.419), vec3(base)), 0.0f);
        }
    }

    else if(convTime >= 8.5f && convTime < 9.0f) {
        float interpolate = clamp((convTime - 8.5f) / 0.5f, 0.0f, 1.0f);
        lightColor = mix(night, sunrise, interpolate);

        for(int i = 0; i < lightSize; i++) {
        if(playerLight[i].y == 0.0f) { break; }
            result += 
                vec4(CalcPointLight(playerLight[i], viewDir, norm, vec3(1, 0.894, 0.419), vec3(base)), 0.0f) * (1.0f - interpolate);
        }
    }

    else if(convTime >= 9.0f && convTime < 9.5f) {
        float interpolate = clamp((convTime - 9.0f) / 0.5f, 0.0f, 1.0f);
        lightColor = mix(sunrise, day, interpolate);
    }

    else if(convTime >= 11.0f) {
        lightColor = day;
    }

    result += vec4(vec3(tex * lightColor * (tex + light + specular + rim)), 1.0f);

    float brightness = dot(vec3(result), vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0) {
        BrightColor = vec4(result);
    }
    else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    FragColor = result;
}