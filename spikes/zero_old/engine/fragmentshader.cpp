#include "fragmentshader.h"
#include <GL/glew.h>

FragmentShader::FragmentShader(const char* source)
: Shader(source, GL_FRAGMENT_SHADER)
{
}
