#ifndef RENDERTARGET_H_
#define RENDERTARGET_H_

#include <memory>
#include "texture.h"
#include <vector>

class RenderTarget
{
public:
	RenderTarget(const unsigned int width, const unsigned int height);
	~RenderTarget();
	void AddTarget(std::shared_ptr<Texture> target);
	void Activate();
private:
	unsigned int fbo;
	unsigned int rbo;
	unsigned int width;
	unsigned int height;
	std::vector<std::shared_ptr<Texture>> targets;
};

#endif

