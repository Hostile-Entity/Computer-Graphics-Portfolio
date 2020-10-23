#pragma once

#include "include/texture.hpp"

class Framebuffer
{
	unsigned fbo;
public:
	Framebuffer() : fbo(0)
	{
	}
	~Framebuffer()
	{
		glDeleteFramebuffers(1, &fbo);
	}

	void initWithTextures(Texture* depthTexture, Texture* texture0 = NULL, Texture* texture1 = NULL);

	void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
	void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};