#version 330 core

uniform sampler2D colortex;
uniform sampler2D normaltex;
uniform sampler2D positiontex;
in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_texcoord;
out vec3 outColor;

const vec3 light_position = vec3(60, 40, 5);

void main(void)
{
	vec3 p = texture2D(positiontex, pass_texcoord).xyz;
	vec3 N = normalize(texture2D(normaltex, pass_texcoord).xyz);

	vec3 L = normalize(light_position - p);
	float lambert = max(0.0f, dot(N, L));

	vec3 color = texture2D(colortex, pass_texcoord).xyz;
	outColor = color * (lambert + 0.1);
//	outColor = N;
}

