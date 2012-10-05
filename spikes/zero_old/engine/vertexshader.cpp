#include "vertexshader.h"
#include <GL/glew.h>

VertexShader::VertexShader(const char* source)
: Shader(source, GL_VERTEX_SHADER)
{
}

