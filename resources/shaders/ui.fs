#version 330 core

uniform sampler2D modeltex;
uniform sampler2D modelnormaltex;
in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_texcoord;
out vec3 outColor[1];

void main(void)
{
	outColor[0] = texture2D(modeltex, pass_texcoord).xyz;
}

