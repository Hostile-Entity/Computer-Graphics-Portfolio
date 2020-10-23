#include "glToolbox.h"

//#############################################################################
// Mesh functions
//#############################################################################
Mesh::~Mesh() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

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

void Mesh::render() const {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}



//#############################################################################
// Shader functions
//#############################################################################
unsigned ShaderProgram::loadShader(const char* path, GLenum type) {
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

void ShaderProgram::loadShader(const char* vpath, const char* fpath) {
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



//#############################################################################
// Texture functions
//#############################################################################
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

void Texture::loadTexture(const char* path) {
	t = SOIL_load_OGL_texture(path, 0, 0, 0);
}



//#############################################################################
// Framebuffer functions
//#############################################################################
void Framebuffer::initWithTextures(Texture* depthTexture, Texture* texture0, Texture* texture1)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	std::vector<GLenum> at;
	if (depthTexture) {
		at.push_back(GL_DEPTH_ATTACHMENT);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture->get(), 0);
	}
	if (texture0) {
		at.push_back(GL_COLOR_ATTACHMENT0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture0->get(), 0);
	}
	if (texture1) {
		at.push_back(GL_COLOR_ATTACHMENT1);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texture1->get(), 0);
	}

	glDrawBuffers(at.size(), at.data());

	glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//#############################################################################
// Object functions
//#############################################################################

void Object::draw(Camera* camera, ShaderProgram* overrideShader, Camera* shadowCamera) const
{
	ShaderProgram* usedShader = overrideShader;
	if (!usedShader) {
		usedShader = shader;
	}

	usedShader->bind();

	int modelMatrixID = usedShader->uniformLocation("modelMatrix");
	int normalMatrixID = usedShader->uniformLocation("normalMatrix");

	glm::mat4 model = glm::translate(glm::mat4(), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(), glm::vec3(scale, scale, scale));
	glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(model)));
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix3fv(normalMatrixID, 1, GL_FALSE, &normal[0][0]);
	
	if (camera) {
		int cameraPosWSID = usedShader->uniformLocation("cameraPosWS");
		int viewMatrixID = usedShader->uniformLocation("viewMatrix");
		int projectionMatrixID = usedShader->uniformLocation("projectionMatrix");

		glm::mat4 view = camera->viewMatrix();
		glm::mat4 projection = camera->projectionMatrix();
		glUniform3f(cameraPosWSID, camera->pos.x, camera->pos.y, camera->pos.z);
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionMatrixID, 1, GL_FALSE, &projection[0][0]);
	}

	if (shadowCamera) {
		int lightPosWSID = usedShader->uniformLocation("lightPosWS");
		int shadowSpaceMatrixID = usedShader->uniformLocation("shadowSpaceMatrix");

		glm::mat4 shadowView = shadowCamera->viewMatrix();
		glm::mat4 shadowProjection = shadowCamera->projectionMatrix();
		glm::mat4 shadowSpace = shadowProjection * shadowView;
		glUniform3f(lightPosWSID, shadowCamera->pos.x, shadowCamera->pos.y, shadowCamera->pos.z);
		glUniformMatrix4fv(shadowSpaceMatrixID, 1, GL_FALSE, &shadowSpace[0][0]);
	}

	if (diffuseMap) {
		diffuseMap->bind(0);
		glUniform1i(usedShader->uniformLocation("diffuseTexture"), 0);
	}
	if (normalMap) {
		normalMap->bind(1);
		glUniform1i(usedShader->uniformLocation("normalTexture"), 1);
	}
	if (heightMap) {
		heightMap->bind(2);
		glUniform1i(usedShader->uniformLocation("heightTexture"), 2);
	}
	if (shadowMap) {
		shadowMap->bind(3);
		glUniform1i(usedShader->uniformLocation("shadowTexture"), 3);
	}

	mesh->render();

	usedShader->unbind();
	if (diffuseMap) {
		diffuseMap->unbind(0);
	}
	if (normalMap)
	{
		normalMap->unbind(1);
	}
	if (heightMap)
	{
		heightMap->unbind(2);
	}
	if (shadowMap)
	{
		shadowMap->unbind(3);
	}
}