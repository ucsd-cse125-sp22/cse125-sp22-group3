#version 460 core

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

in VS_OUT
{
    vec2 v_TexCoord;
} gs_in[];

out GS_OUT
{
    vec2 v_TexCoord;
} gs_out;

void main()
{          
	for (int i = 0; i < 3; ++i)
	{
		gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		gs_out.v_TexCoord = gs_in[i].v_TexCoord;
		EmitVertex();
	}
	EndPrimitive();
}  
