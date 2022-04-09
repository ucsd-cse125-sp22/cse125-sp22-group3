#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

// You can output many things. The first vec4 type output determines the color of the fragment
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

out vec4 fragColor;

void main()
{
    /*
    vec3 viewPos = vec3(0.0f, 0.0f, 0.20f);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lightPos = vec3(0.7f, 0.2f, 2.0f);
    vec3 lightDir = normalize(lightPos - FragPos);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    // attenuation
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0f + 0.09f * distance + 0.032f * (distance * distance));    
    // combine results
    vec3 ambient = vec3(0.05f, 0.05f, 0.05f) * vec3(texture(diffuse1, TexCoords));
    vec3 diffuse = vec3(0.8f, 0.8f, 0.8f) * diff * vec3(texture(diffuse1, TexCoords));
    vec3 specular = vec3(1.0f, 1.0f, 1.0f) * spec * vec3(texture(specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    */

    // Use the color passed in. An alpha of 1.0f means it is not transparent.
    fragColor = texture(texture_diffuse1, TexCoords);
    // fragColor = vec4(1.0f);
    // fragColor = vec4(norm, 1.0f);
}