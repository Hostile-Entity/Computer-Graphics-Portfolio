#include "include/mesh.hpp"

void Mesh::load(const char* path) {
	Assimp::Importer imp;
	const aiScene* sc = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);

	const aiMesh* m = sc->mMeshes[0];
	for (int i = 0; i < m->mNumVertices; i += 1) {
		MeshVertex vertex;
		vertex.position = glm::vec3(m->mVertices[i].x, m->mVertices[i].y, m->mVertices[i].z);
		vertex.normal = glm::vec3(m->mNormals[i].x, m->mNormals[i].y, m->mNormals[i].z);
		vertex.tangent = glm::vec3(m->mTangents[i].x, m->mTangents[i].y, m->mTangents[i].z);
		vertex.texCoords = glm::vec2(m->mTextureCoords[0][i].x, m->mTextureCoords[0][i].y);
		vertices.push_back(vertex);
	}
	for (int i = 0; i < m->mNumFaces; i += 1) {
		const aiFace& f = m->mFaces[i];
		for (int j = 0; j < f.mNumIndices; ++j) {
			indices.push_back((unsigned) f.mIndices[j]);
		}
	}

	setup();
}

void Mesh::setup() {
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, tangent));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, texCoords));

	glBindVertexArray(0);
}