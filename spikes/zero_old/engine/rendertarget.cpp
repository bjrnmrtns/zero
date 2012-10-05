#include "rendertarget.h"
#include <GL/glew.h>

static const unsigned int Attachments[] =
{
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8,
	GL_COLOR_ATTACHMENT9,
	GL_COLOR_ATTACHMENT10,
	GL_COLOR_ATTACHMENT11,
	GL_COLOR_ATTACHMENT12,
	GL_COLOR_ATTACHMENT13,
	GL_COLOR_ATTACHMENT14,
	GL_COLOR_ATTACHMENT15
};

RenderTarget::RenderTarget(const unsigned int width, const unsigned int height)
: fbo(0)
, rbo (0)
, width(width)
, height(height)
{
}
RenderTarget::~RenderTarget()
{
	if(targets.size() > 0)
	{
		glDeleteRenderbuffers(1, &rbo);
		glDeleteFramebuffers(1, &fbo);
	}
}
void RenderTarget::AddTarget(std::shared_ptr<Texture> target)
{
	if(targets.size() == 0)
	{
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	target->SetAsAttachment(targets.size());
	targets.push_back(target);
}
void RenderTarget::Activate()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	if(targets.size() > 0)
	{
		glDrawBuffers(targets.size(), Attachments);
		for(unsigned int i=0; i < targets.size(); i++)
		{
			targets[i]->SetAsAttachment(i);
		}
	}
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

