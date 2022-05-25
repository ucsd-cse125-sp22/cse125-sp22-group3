#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 texCoords;

uniform sampler2D texture_diffuse1;
uniform float time;

const vec3 day = vec3(0.8, 0.7, 0.6);
const vec3 night = vec3(0, 0.027, 0.121);
const vec3 sunset = vec3(0.701, 0.337, 0.078);
const vec3 sunrise = vec3(0.890, 0.290, 0.439);
// You can output many things. The first vec4 type output determines the color of the fragment

void main()
{
    vec4 tex =  texture(texture_diffuse1, texCoords);

    float secondsToMinutes = time / 60.0f;
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

    FragColor = vec4(lightColor, 1.0f) * tex;
    BrightColor  = FragColor * vec4(0.9f);
}