#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

// You can output many things. The first vec4 type output determines the color of the fragment
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform sampler2D shadowMap;

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

    // shadows
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;   
    float currentDepth = projCoords.z;  
    float bias =  max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);  

    float shadow = 0.0f; 
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    int halfkernelWidth = 3;
    for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
    {
	    for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
	    {
		    float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
		    shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	    }
    }

    shadow /= ((halfkernelWidth*2+1)*(halfkernelWidth*2+1));

    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }


    // diffuse shading
    float NdotL = dot(norm, lightPos);
    // float lightIntensity = smoothstep(0, 0.01, NdotL * (1 - shadow));
    float lightIntensity = smoothstep(0, 0.01, NdotL);
    vec4 light = lightIntensity * vec4(0.5f);

    // specular
    float NdotH = dot(norm, halfVector);
    float specularIntensity = pow(NdotH * lightIntensity, 32);
    float specularIntensitySmooth = smoothstep(0.005, 0.01, specularIntensity);
    vec4 specular = specularIntensitySmooth * vec4(0.1f, 0.1f, 0.1f, 1.0f);

    float rimDot = 1 - dot(viewDir, norm);
    float rimIntensity = rimDot * pow(NdotL, 0.1);
    // rimIntensity = smoothstep(0.716 - 0.01, 0.716 + 0.01, rimIntensity);
    vec4 rim = rimIntensity * vec4(1.0);

    // fragColor = tex * vec4(0.8, 0.7, 0.6, 1.0) * (tex + light + specular + rim);
    fragColor = tex * vec4(0.6, 0.5, 0.4, 1.0) * (tex + light + specular + rim);

    /*
    // Realistic
    vec4 tex = texture(texture_diffuse1, TexCoords);

    if(tex.a < 0.8) {
        discard;
    }

    // Directional Light
    vec3 lightPos = vec3(-2.0f, 2.0f, -1.0f);
    vec3 lightDir = normalize(lightPos);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);   

    // combine results
    vec3 ambient = vec3(0.5f, 0.5f, 0.5f) * vec3(tex);
    vec3 diffuse = vec3(0.6f, 0.6f, 0.6f) * diff * vec3(tex);
    vec3 specular = vec3(0.1f, 0.1f, 0.1f) * spec * vec3(tex);

    // shadows
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;   
    float currentDepth = projCoords.z;  
    float bias =  max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);  

    float shadow = 0.0f; 
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    int halfkernelWidth = 3;
    for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
    {
	    for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
	    {
		    float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
		    shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	    }
    }

    shadow /= ((halfkernelWidth*2+1)*(halfkernelWidth*2+1));

    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }
 
    // check for transparency;
    fragColor = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0f);
    */
}