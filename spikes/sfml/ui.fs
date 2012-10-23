#version 330 core

uniform sampler2D uitexture;
in vec2 pass_position;
in vec4 pass_color;
in vec2 pass_texcoord;
out vec4 outColor[1];

void main(void)
{
	outColor[0] = texture2D(uitexture, pass_texcoord);
}

