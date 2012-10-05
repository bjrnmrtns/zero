#ifndef SHADERPROGRAM_
#define SHADERPROGRAM_

#include "vertexshader.h"
#include "fragmentshader.h"
#include <memory>

class ShaderProgram
{
public:
	ShaderProgram(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<FragmentShader> fragmentShader);
	~ShaderProgram();
	void Set(const char *name, const float mat[16]);
	bool SetTexture(const char *name, int value);
	void Use();
private:
	int id;
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<FragmentShader> fragmentShader;
};

#endif
