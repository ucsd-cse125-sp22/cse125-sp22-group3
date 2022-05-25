#version 460 core

#define PI 3.1415926538

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
in float t;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

const vec3 day = vec3(0.6, 0.5, 0.4);
const vec3 night = vec3(0, 0.027, 0.121);
const vec3 sunset = vec3(0.701, 0.337, 0.078);
const vec3 sunrise = vec3(0.890, 0.290, 0.439);

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};
uniform float cascadePlaneDistances[16];

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

    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);   

    float secondsToMinutes = t / 60.0f;
    float convTime = clamp(secondsToMinutes, 0.0f, 15.0f);
    vec3 lightColor = day;

    if(convTime >= 2.5f && convTime < 3.0f) {
        float interpolate = clamp((convTime - 2.5f) / 0.5f, 0.0f, 1.0f);
        lightColor = mix(day, sunset, interpolate);
    }

    else if(convTime >= 3.0f && convTime < 3.5f) {
        float interpolate = clamp((convTime - 3.0f) / 0.5f, 0.0f, 1.0f);
        lightColor = mix(sunset, night, interpolate);
    }

    else if(convTime >= 3.5f && convTime < 8.5f) {
        lightColor = night;
    }

    else if(convTime >= 8.5f && convTime < 9.0f) {
        float interpolate = clamp((convTime - 8.5f) / 0.5f, 0.0f, 1.0f);
        lightColor = mix(night, sunrise, interpolate);
    }

    else if(convTime >= 9.0f && convTime < 9.5f) {
        float interpolate = clamp((convTime - 9.0f) / 0.5f, 0.0f, 1.0f);
        lightColor = mix(sunrise, day, interpolate);
    }

    else if(convTime >= 9.5f) {
        lightColor = day;
    }

    // combine results
    vec3 ambient = lightColor * vec3(tex);
    vec3 diffuse = lightColor * diff * vec3(tex);
    vec3 specular = vec3(0.1f, 0.1f, 0.1f) * spec * vec3(tex);
 

    vec4 result = vec4(ambient + diffuse + specular, 1.0f);
    float brightness = dot(vec3(result), vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0) {
        BrightColor = result;
    }
    else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    // check for transparency;
    FragColor = result;
}