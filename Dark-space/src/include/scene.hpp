#pragma once

#include "include/lib.h"
#include "include/mesh.hpp"
#include "include/camera.hpp"
#include "include/texture.hpp"
#include "include/frameBuffer.hpp"
#include "include/shader.hpp"
#include "include/object.hpp"

class Scene
{
public:
	GLFWwindow* window;
	float time;
	float delta;
private:
	Mesh framebufferQuadMesh;

	Camera camera;

	Camera shadowCamera;
	Framebuffer shadowMapFramebuffer;
	Texture shadowMapTexture;
	ShaderProgram shadowGenShaderProgram;


	Object skybox;
	Mesh skyboxMesh;
	Texture skyboxCubemap;
	ShaderProgram skyboxShaderProgram;

	Object earth;
	Mesh earthMesh;
	Texture earthTexture;
	Texture earthNormalMap;
	Texture earthHeightMap;
	ShaderProgram earthShaderProgram;

	Object spaceship;
	Mesh spaceshipMesh;
	ShaderProgram spaceshipShaderProgram;

	Object asteroid;
	Mesh asteroidMesh;
	Texture asteroidTexture;
	Texture asteroidNormalMap;
	Texture asteroidHeightMap;
public:
	void init();

	void clear()
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void render();

	void interface();
};