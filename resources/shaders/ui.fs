#version 330 core

uniform sampler2D modeltex;
in vec2 pass_position;
in vec4 pass_color;
in vec2 pass_texcoord;
out vec3 outColor[1];

void main(void)
{
	outColor[0] = texture2D(modeltex, pass_texcoord).xyz;
//	outColor[0] = pass_color.xyz;
}

