#include "include/frameBuffer.hpp"

void Framebuffer::initWithTextures(Texture* depthTexture, Texture* texture0, Texture* texture1)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	std::vector<GLenum> at;
	if (depthTexture) {
		at.push_back(GL_DEPTH_ATTACHMENT);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture->get(), 0);
	}
	if (texture0) {
		at.push_back(GL_COLOR_ATTACHMENT0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture0->get(), 0);
	}
	if (texture1) {
		at.push_back(GL_COLOR_ATTACHMENT1);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texture1->get(), 0);
	}

	glDrawBuffers(at.size(), at.data());

	glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}