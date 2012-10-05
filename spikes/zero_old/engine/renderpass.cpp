#include "renderpass.h"

RenderPass::RenderPass(std::shared_ptr<VertexShader> vertexshader, std::shared_ptr<FragmentShader> fragmentshader, const unsigned int width, const unsigned int height)
: rendertarget(new RenderTarget(width, height))
, vertexshader(vertexshader)
, fragmentshader(fragmentshader)
, shaderprogram(vertexshader, fragmentshader)
{
}

void RenderPass::Render(Renderable& renderable)
{
	rendertarget->Activate();
	renderable.Draw(shaderprogram);
}
