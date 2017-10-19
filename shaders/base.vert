#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;

layout (binding = 0) uniform UboView 
{
	mat4 projection;
	mat4 view;
	vec3 lightPos;
} uboView;

layout (binding = 1) uniform UboInstance 
{
	mat4 model; 
} uboInstance;

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
	vec4 worldPos = uboInstance.model * vec4(inPos, 1.0);

	outColor = inColor;
	outNormal = mat3(uboInstance.model) * inNormal;
	light = uboView.lightPos - worldPos.xyz;
	view = (uboView.view * worldPos).xyz;

	gl_Position = uboView.projection * uboView.view * uboInstance.model * vec4(inPos, 1.0);
}