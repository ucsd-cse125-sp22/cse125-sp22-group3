#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

// You can output many things. The first vec4 type output determines the color of the fragment
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

out vec4 fragColor;

void main()
{
    /*
    vec3 lightPos = vec3(0.0f, 5.0f, 5.0f);
    vec3 lightDir = normalize(lightPos);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 60.0f);   

    // combine results
    vec3 ambient = vec3(0.05f, 0.05f, 0.05f) * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = vec3(0.6f, 0.6f, 0.6f) * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = vec3(0.5f, 0.5f, 0.5f) * spec * vec3(texture(texture_diffuse1, TexCoords));
    fragColor = vec4(ambient + diffuse + specular, 1.0f);

    // Point light
    // vec3 lightDir = normalize(lightPos - FragPos);
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0f + 0.09f * distance + 0.032f * (distance * distance)); 

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    */

    
    // Use the color passed in. An alpha of 1.0f means it is not transparent.
    fragColor = texture(texture_diffuse1, TexCoords);
    // fragColor = vec4(1.0f);
}