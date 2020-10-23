#pragma once

#include <vector>
#include <map>
#include <cstring>
#include <cstdio>
#include <string>


#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

static const size_t WATER_WIDTH = 100;
static const float RELAX_W = 1.97;

using namespace glm;

bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
);

void indexVBO(
	std::vector<glm::vec3>& in_vertices,
	std::vector<glm::vec2>& in_uvs,
	std::vector<glm::vec3>& in_normals,

	std::vector<unsigned short>& out_indices,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
);

GLuint loadBMP_custom(const char* imagepath);

void newNormals(std::vector<vec3>& indexed_vertices, std::vector<vec3>& indexed_normals);

void simulation(std::vector<glm::vec3>& A, std::vector<glm::vec3>& B, std::vector<glm::vec3>& C);

void rotateVectors(std::vector<glm::vec3>& A, std::vector<glm::vec3>& B, std::vector<glm::vec3>& C);