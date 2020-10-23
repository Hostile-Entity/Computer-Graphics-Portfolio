#include "include/texture.hpp"

void Texture::init(int format1, GLsizei w, GLsizei h, GLenum format2, GLenum type) {
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexImage2D(GL_TEXTURE_2D, 0, format1, w, h, 0, format2, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::loadTexture(const char* path)
{
	iscube = false;
	t = SOIL_load_OGL_texture(path, 0, 0, 0);
}

void Texture::loadCubeTexture(const char* dir, const char* ext)
{
	const int MAX_PATH_LENGTH = 100;
	iscube = true;
	char posX[MAX_PATH_LENGTH] = {};
	char negX[MAX_PATH_LENGTH] = {};
	char posY[MAX_PATH_LENGTH] = {};
	char negY[MAX_PATH_LENGTH] = {};
	char posZ[MAX_PATH_LENGTH] = {};
	char negZ[MAX_PATH_LENGTH] = {};

	sprintf(posX, "%s/posx%s", dir, ext);
	sprintf(negX, "%s/negx%s", dir, ext);
	sprintf(posY, "%s/posy%s", dir, ext);
	sprintf(negY, "%s/negy%s", dir, ext);
	sprintf(posZ, "%s/posz%s", dir, ext);
	sprintf(negZ, "%s/negz%s", dir, ext);

	t = SOIL_load_OGL_cubemap(posX, negX, posY, negY, posZ, negZ, 0, 0, 0);
}