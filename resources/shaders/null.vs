#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;
out vec3 pass_position;
out vec3 pass_normal;
out vec2 pass_texcoord;

void main()
{
	gl_Position = vec4(in_position, 1.0);
	pass_position = in_position;
	pass_normal = in_normal;
	pass_texcoord = in_texcoord;
}
