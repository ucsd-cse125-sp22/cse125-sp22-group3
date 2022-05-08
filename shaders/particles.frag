#version 330 core

in vec2 texCoords;

uniform sampler2D texture_diffuse1;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
    vec4 tex = texture(texture_diffuse1, texCoords);

    
    if(tex.a < 0.05f) {
        discard;
    }

    else {
        fragColor = tex;
    }
    

    // fragColor = tex;
}