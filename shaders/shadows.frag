#version 460 core
out vec4 FragColor;

in GS_OUT
{
    vec2 v_TexCoord;
} fs_in;

uniform bool hasTexture;
uniform sampler2D diffuse;

void main()
{
	if(hasTexture) {
		vec4 tex = texture(diffuse, fs_in.v_TexCoord);

		if(tex.a < 0.8) {
			discard;
		}

		else {
			gl_FragDepth = gl_FragCoord.z;
		}
	}

	else {
		discard;
	}
}