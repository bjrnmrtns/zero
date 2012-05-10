#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec2 in_texcoord;
layout (location = 3) uniform mat4 world;
out vec2 pass_position;
out vec4 pass_color;
out vec2 pass_texcoord;

void main(void)
{
	gl_Position = vec4(in_position, 0.0f, 1.0f);
	pass_position = in_position;
	pass_color = in_color;
	pass_texcoord = in_texcoord;
}

