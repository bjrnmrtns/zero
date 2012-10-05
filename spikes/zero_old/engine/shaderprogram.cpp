#include "shaderprogram.h"
#include <GL/glew.h>
#include <cstdio>
#include <cstdlib>

ShaderProgram::ShaderProgram(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<FragmentShader> fragmentShader)
: vertexShader(vertexShader)
, fragmentShader(fragmentShader)
{
	id = glCreateProgram();
	glAttachShader(id, vertexShader->GetId());
	glAttachShader(id, fragmentShader->GetId());
	
	glBindAttribLocation(id, 0, "position");
	glBindAttribLocation(id, 1, "normal");
	glBindAttribLocation(id, 2, "texcoord");
	glBindFragDataLocation(id, 0, "output");
	glLinkProgram(id);
}
ShaderProgram::~ShaderProgram()
{
	glDetachShader(id, fragmentShader->GetId());
	glDetachShader(id, vertexShader->GetId());
	glDeleteProgram(id);
}
void ShaderProgram::Set(const char *name, const float mat[16])
{
	int uniform = glGetUniformLocation(id, name);
	if (uniform == -1) return;
	glUseProgram(id);
	glUniformMatrix4fv(uniform, 1, GL_FALSE, mat);
	int error = glGetError(); if (error) { std::printf("%d\n", error); abort(); }
}
bool ShaderProgram::SetTexture(const char *name, int value)
{
	int error;
	int uniform = glGetUniformLocation(id, name);
	error = glGetError(); if (error) { std::printf("%d\n", error); abort(); }
	if (uniform == -1) return false;

	glUseProgram(id);
	glUniform1i(uniform, value);
	error = glGetError(); if (error) { printf("%d\n", error); abort(); }
	return true;
}
void ShaderProgram::Use()
{
	glUseProgram(id);
}

