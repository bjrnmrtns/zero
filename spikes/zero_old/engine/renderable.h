#ifndef RENDERABLE_
#define RENDERABLE_

#include "shaderprogram.h"
#include <GL/glew.h>

class Renderable
{
public:
	virtual void Draw(ShaderProgram& shaderProgram, unsigned int type = GL_TRIANGLES) = 0;
	virtual ~Renderable() {};
};

#endif
