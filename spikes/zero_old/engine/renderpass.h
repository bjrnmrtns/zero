#ifndef RENDERPASS_H_
#define RENDERPASS_H_

#include <memory>
#include "shaderprogram.h"
#include "rendertarget.h"
#include "renderable.h"
#include "vertexshader.h"
#include "fragmentshader.h"

class RenderPass
{
public:
	RenderPass(std::shared_ptr<VertexShader> vertexshader, std::shared_ptr<FragmentShader> fragmentshader, const unsigned int width, const unsigned int height);
	void Render(Renderable& renderable);
protected:
	std::shared_ptr<RenderTarget> rendertarget;
	std::shared_ptr<VertexShader> vertexshader;
	std::shared_ptr<FragmentShader> fragmentshader;
	ShaderProgram shaderprogram;
};

#endif

