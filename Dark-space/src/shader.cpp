#include "include/shader.hpp"

unsigned ShaderProgram::loadShader(const char* path, GLenum type)
{
	FILE* f = fopen(path, "r");


	fseek(f, 0, SEEK_END);
	long s = ftell(f);
	fseek(f, 0, SEEK_SET);

	std::vector<char> text(s + 1);
	fread(text.data(), sizeof(text[0]), s, f);
	fclose(f);

	unsigned shader = glCreateShader(type);
	const char* ptext = text.data();
	glShaderSource(shader, 1, &ptext, NULL);
	glCompileShader(shader);

	int res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);

	return shader;
}

void ShaderProgram::loadShader(const char* vpath, const char* fpath)
{
	unsigned vsh = loadShader(vpath, GL_VERTEX_SHADER);
	unsigned fsh = loadShader(fpath, GL_FRAGMENT_SHADER);

	sp = glCreateProgram();
	glAttachShader(sp, vsh);
	glAttachShader(sp, fsh);
	glLinkProgram(sp);

	int res;
	glGetProgramiv(sp, GL_LINK_STATUS, &res);

	glDeleteShader(vsh);
	glDeleteShader(fsh);
}