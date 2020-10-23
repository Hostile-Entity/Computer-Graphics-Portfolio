#pragma once

#include "include/lib.h"
#include "include/camera.hpp"
#include "include/mesh.hpp"
#include "include/texture.hpp"
#include "include/shader.hpp"

class Object
{
public:
	Mesh* mesh;
	ShaderProgram* shader;
	Texture* diffuseMap;
	Texture* normalMap;
	Texture* heightMap;
	Texture* shadowMap;
	glm::vec3 position;
	glm::quat rotation;
	float speed;
	float scale;

	Object() : mesh(NULL), shader(NULL), diffuseMap(NULL), normalMap(NULL), heightMap(NULL), shadowMap(NULL), scale(1.0f), speed(1.0f)
	{
	}

	void draw(Camera* camera, ShaderProgram* overrideShader = NULL, Camera* shadowCamera = NULL) const;
};