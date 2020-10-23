#pragma once

#include "include/lib.h"

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
	~Mesh() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}

	void load(const char* path);

	void setup();

	void Mesh::render() const {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);
	}
};