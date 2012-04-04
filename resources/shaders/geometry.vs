#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;
layout (location = 3) uniform mat4 projection;
layout (location = 4) uniform mat4 view;
out vec3 pass_position;
out vec3 pass_normal;
out vec2 pass_texcoord;

void main(void)
{
	gl_Position = projection * view * vec4(in_position, 1.0);
	pass_position = gl_Position.xyz;
	pass_normal = in_normal;
	pass_texcoord = in_texcoord;
}
