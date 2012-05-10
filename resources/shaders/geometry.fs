#version 330 core

uniform sampler2D modeltex;
in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_texcoord;
out vec3 outColor[3];

void main(void)
{
	outColor[0] = pass_position;
	outColor[1] = texture2D(modeltex, pass_texcoord).xyz;
	outColor[2] = pass_normal;
}

