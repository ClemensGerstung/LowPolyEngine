#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;

layout (location = 3) in vec4 inRow1;
layout (location = 4) in vec4 inRow2;
layout (location = 5) in vec4 inRow3;
layout (location = 6) in vec4 inRow4;

//layout (location = 3) in mat4 inMatrix;

layout (binding = 0) uniform UboView 
{
	mat4 projection;
	mat4 view;
	vec3 lightPos;
} uboView;


layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 view;
layout (location = 3) out vec3 light;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() 
{
	mat4 inMatrix;
	inMatrix[0] = inRow1;
	inMatrix[1] = inRow2;
	inMatrix[2] = inRow3;
	inMatrix[3] = inRow4;

	vec4 worldPos = inMatrix * vec4(inPos, 1.0);

	outColor = inColor;
	outNormal = mat3(inMatrix) * inNormal;
	light = uboView.lightPos - worldPos.xyz;
	view = (uboView.view * worldPos).xyz;

	gl_Position = uboView.projection * uboView.view * inMatrix * vec4(inPos, 1.0);
}
