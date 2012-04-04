#version 330 core

uniform sampler2D colortex;
uniform sampler2D normaltex;
in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_texcoord;
out vec3 outColor;

void main(void)
{
	vec3 tmp = texture2D(normaltex, pass_texcoord).xyz;
	outColor = vec3(tmp.x + 0.5, tmp.xy);
}

