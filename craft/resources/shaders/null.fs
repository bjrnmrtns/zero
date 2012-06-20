#version 330 core

uniform sampler2D modeltex;
in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_texcoord;
out vec3 outColor;

const vec3 light_position = vec3(60, 40, 5);

void main(void)
{
	vec3 p = pass_position;
	vec3 N = normalize(pass_normal.xyz);

	vec3 L = normalize(light_position - p);
	float lambert = max(0.0f, dot(N, L));

	outColor = vec3(1.0f, 1.0f, 0.0f) * (lambert + 0.1);
	//outColor = vec3(1.0f, 1.0f, 0.0f);
}

