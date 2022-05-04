#version 460 core
out vec4 FragColor;
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

// You can output many things. The first vec4 type output determines the color of the fragment
in vec2 TexCoords;

// uniform sampler2DArray map;
uniform sampler2D map;

out vec4 fragColor;

void main()
{    
    /*
    float depthValue = texture(map, vec3(TexCoords, 0)).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    fragColor = vec4(vec3(depthValue), 1.0); // orthographic
    */
    fragColor = texture(map, TexCoords);
}