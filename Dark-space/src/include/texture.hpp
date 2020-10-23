#pragma once

#include "include/lib.h"

class Texture
{
	unsigned t;
	bool iscube;

public:
	Texture() : t(0), iscube(false)
	{
	}
	~Texture()
	{
		glDeleteTextures(1, &t);
	}

	unsigned get() const
	{
		return t;
	}

	void loadTexture(const char* path);

	void Texture::loadCubeTexture(const char* dir, const char* ext);

	void init(int format1, GLsizei w, GLsizei h, GLenum format2, GLenum type);

	void initColor(GLsizei w = SCREEN_WIDTH, GLsizei h = SCREEN_HEIGHT)
	{
		init(GL_RGB, w, h, GL_RGB, GL_FLOAT);
	}
	void initDepth(GLsizei w = SCREEN_WIDTH, GLsizei h = SCREEN_HEIGHT)
	{
		init(GL_DEPTH_COMPONENT, w, h, GL_DEPTH_COMPONENT, GL_FLOAT);
	}

	void bind(int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, t);
	}
	void unbind(int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};