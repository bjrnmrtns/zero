#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;
layout (location = 3) in vec3 in_color;
layout (location = 4) uniform mat4 projection;
layout (location = 5) uniform mat4 view;
layout (location = 6) uniform mat4 world;
out vec3 pass_position;
out vec3 pass_normal;
out vec2 pass_texcoord;
out vec3 pass_color;

void main(void)
{
	gl_Position = projection * view * world * vec4(in_position, 1.0);
	pass_position = in_position;
	pass_normal = in_normal;
	pass_texcoord = in_texcoord;
	pass_color = in_color;
}

