#pragma once

#include "include/lib.h"

class ShaderProgram
{
	unsigned sp;

public:
	ShaderProgram() : sp(0)
	{
	}
	~ShaderProgram()
	{
		glDeleteProgram(sp);
	}

private:
	static unsigned loadShader(const char* path, GLenum type);

public:
	void loadShader(const char* vpath, const char* fpath);

	int uniformLocation(const char* s) const
	{
		return glGetUniformLocation(sp, s);
	}

	void bind() const
	{
		glUseProgram(sp);
	}
	void unbind()
	{
		glUseProgram(0);
	}
};