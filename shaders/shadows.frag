#version 460 core
out vec4 FragColor;

void main()
{
	gl_FragDepth = gl_FragCoord.z;
}