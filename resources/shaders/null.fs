#version 330 core

uniform sampler2D modeltex;
in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_texcoord;
out vec3 outColor;

void main(void)
{
	outColor = texture2D(modeltex, pass_texcoord).xyz;
}

