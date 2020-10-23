#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL2.h>

#include <stdio.h>
#include <vector>

//Screen dimension
#define WIDTH 1366
#define HEIGHT 768

struct MeshVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 texCoords;
};

class Mesh
{
	unsigned vbo, ebo, vao;
public:
	std::vector<MeshVertex> vertices;
	std::vector<unsigned> indices;

	Mesh(): vbo(0), ebo(0), vao(0)
	{
	}
	~Mesh();

	void load(const char* path);

	void setup();

	void render() const;
};


class ShaderProgram
{
	unsigned sp;

public:
	ShaderProgram(): sp(0)
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

class Texture
{
	unsigned t;

public:
	Texture(): t(0)
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

	void init(int format1, GLsizei w, GLsizei h, GLenum format2, GLenum type);
	
	void initColor(GLsizei w = WIDTH, GLsizei h = HEIGHT)
	{
		init(GL_RGB, w, h, GL_RGB, GL_FLOAT);
	}
	void initDepth(GLsizei w = WIDTH, GLsizei h = HEIGHT)
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


class Camera
{
public:
	glm::vec3 pos;
	glm::quat rot;
	float distance;

	glm::mat4 viewMatrix() const
	{
		glm::mat4 view = glm::lookAt(pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		return view;
	}
	glm::mat4 projectionMatrix() const
	{
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 300.0f);
		return projection;
	}
};

class Object
{
public:
	Mesh* mesh;
	Texture* diffuseMap;
	Texture* normalMap;
	Texture* heightMap;
	Texture* shadowMap;
	ShaderProgram* shader;
	glm::vec3 position;
	glm::quat rotation;
	float scale;

	Object(): mesh(NULL), shader(NULL), diffuseMap(NULL), normalMap(NULL), heightMap(NULL), shadowMap(NULL), scale(1.0f)
	{
	}

	void draw(Camera* camera, ShaderProgram* overrideShader = NULL, Camera* shadowCamera = NULL) const;
};