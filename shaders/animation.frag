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

/*
// GGX normal distribution
float D(float alpha, vec3 normal, vec3 h) {
    float nume = pow(alpha, 2.0);

    float dot = max(dot(normal, h), 0.0);
    float deno = 3.1415926 * pow(pow(dot, 2.0) * (pow(alpha, 2.0) - 1.0) + 1.0, 2.0);
    deno = max(deno, 0.000001);

    return nume / deno;
}

// Geometry Shadowing (Schlick - Beckham)
float G1(float alpha, vec3 N, vec3 X) {
    float nume = max(dot(N, X), 0.0);

    float k = alpha / 2.0f;
    float deno = max(dot(N, X), 0.0) * (1.0 - k) + k;
    deno = max(deno, 0.000001);

    return nume / deno;
}

// Smith Model
float G(float alpha, vec3 N, vec3 V, vec3 L) {
    return G1(alpha, N, V) * G1(alpha, N, L);
}

// Freshnel - Schleck
vec3 F(vec3 F0, vec3 V, vec3 H) {
    return F0 + (vec3(1.0) - F0) * pow(1 - max(dot(V, H), 0.0), 5.0);
}

vec3 PBR() {
    vec3 N = normalize(Normal);
    vec3 lightPos = vec3(0.0f, 5.0f, 5.0f);
    vec3 V = normalize(viewPos - FragPos);
    vec3 L = normalize(lightPos);
    vec3 H = normalize(V + L);

    vec3 F0 = vec3(0.04); 
    vec3 ks = F(F0, V, H);
    vec3 kd = vec3(1.0) - ks;
    float alpha = 0.0f;

    vec3 lambert = pow(texture(texture_diffuse1, TexCoords).rgb, 2.2) / 3.1415926;

    vec3 cookTorrenceNum = D(alpha, N, H) * G(alpha, N, V, L) * F(F0, V, H);
    float ctDen = 4.0 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0);
    ctDen = max(ctDen, 0.000001);
    vec3 cookTorrence = cookTorrenceNum / ctDen;

    vec3 BRDF = kd * lambert * cookTorrence;
    vec3 outgoingLight = emissivityMesh + BRDF * vec3(0.5f, 0.5f, 0.5f) * max(dot(L, N), 0.0);
    
    return outgoingLight;
}
*/
void main()
{
    // Directional Light
    vec3 lightPos = vec3(0.0f, 5.0f, 5.0f);
    vec3 lightDir = normalize(lightPos);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);   


    vec4 tex = texture(texture_diffuse1, TexCoords);
    // combine results
    vec3 ambient = vec3(0.5f, 0.5f, 0.5f) * vec3(tex);
    vec3 diffuse = vec3(0.6f, 0.6f, 0.6f) * diff * vec3(tex);
    vec3 specular = vec3(0.1f, 0.1f, 0.1f) * spec * vec3(tex);

    if(tex.a < 0.8) {
        discard;
    }

    // check for transparency;
    fragColor = vec4(ambient + diffuse + specular, 1.0f);

    /*
    // Point light
    // vec3 lightDir = normalize(lightPos - FragPos);
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0f + 0.09f * distance + 0.032f * (distance * distance)); 

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    */

    /*
    // Use the color passed in. An alpha of 1.0f means it is not transparent.
    fragColor = texture(texture_diffuse1, TexCoords);
    // fragColor = vec4(1.0f);
    */
}