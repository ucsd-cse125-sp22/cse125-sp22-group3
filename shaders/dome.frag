#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 texCoords;

uniform sampler2D texture_diffuse1;

const vec3 day = vec3(0.8, 0.7, 0.6);
const vec3 night = vec3(0, 0.027, 0.121);
const vec3 sunset = vec3(0.701, 0.337, 0.078);
const vec3 sunrise = vec3(0.890, 0.290, 0.439);
// You can output many things. The first vec4 type output determines the color of the fragment

void main()
{
    vec4 tex =  texture(texture_diffuse1, texCoords);
    FragColor = vec4(day, 1.0f) * tex;
    BrightColor  = FragColor * vec4(0.9f);
}