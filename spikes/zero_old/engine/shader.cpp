#include "shader.h"
#include "shadercompileexception.h"
#include <GL/glew.h>

Shader::Shader(const char* source, int type)
: type(type)
{
	id = glCreateShader(type);
	glShaderSource(id, 1, &source, 0);
	glCompileShader(id);
	int ok = true;
	glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
	if(!ok)
	{
		int length = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char errors[length];
		glGetShaderInfoLog(id, length, 0, errors);
		throw ShaderCompileException(std::string(errors));
	}
}

Shader::~Shader()
{
	glDeleteShader(id);
}

int Shader::GetId()
{ 
	return id; 
}
