#include "include/object.hpp"

void Object::draw(Camera* camera, ShaderProgram* overrideShader, Camera* shadowCamera) const
{
	ShaderProgram* usedShader = overrideShader;
	if (!usedShader)
	{
		usedShader = shader;
	}

	usedShader->bind();

	int modelMatrixID = usedShader->uniformLocation("modelMatrix");

	int normalMatrixID = usedShader->uniformLocation("normalMatrix");

	glm::mat4 model = glm::translate(glm::mat4(), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(), glm::vec3(scale, scale, scale));
	glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(model)));

	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix3fv(normalMatrixID, 1, GL_FALSE, &normal[0][0]);

	if (camera)
	{
		int cameraPosWSID = usedShader->uniformLocation("cameraPosWS");
		int viewMatrixID = usedShader->uniformLocation("viewMatrix");
		int projectionMatrixID = usedShader->uniformLocation("projectionMatrix");

		glm::mat4 view = camera->viewMatrix();
		glm::mat4 projection = camera->projectionMatrix();
		glUniform3f(cameraPosWSID, camera->position.x, camera->position.y, camera->position.z);
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionMatrixID, 1, GL_FALSE, &projection[0][0]);
	}

	if (shadowCamera)
	{
		int lightPosWSID = usedShader->uniformLocation("ligth_pos_world_space");
		int shadowSpaceMatrixID = usedShader->uniformLocation("shadowSpaceMatrix");

		glm::mat4 shadowView = shadowCamera->viewMatrix();
		glm::mat4 shadowProjection = shadowCamera->projectionMatrix();
		glm::mat4 shadowSpace = shadowProjection * shadowView;

		glUniform3f(lightPosWSID, shadowCamera->position.x, shadowCamera->position.y, shadowCamera->position.z);
		glUniformMatrix4fv(shadowSpaceMatrixID, 1, GL_FALSE, &shadowSpace[0][0]);
	}

	if (diffuseMap)
	{
		diffuseMap->bind(0);
		glUniform1i(usedShader->uniformLocation("diffuseTexture"), 0);
	}
	if (normalMap)
	{
		normalMap->bind(1);
		glUniform1i(usedShader->uniformLocation("normalTexture"), 1);
	}
	if (heightMap)
	{
		heightMap->bind(2);
		glUniform1i(usedShader->uniformLocation("heightTexture"), 2);
	}
	if (shadowMap)
	{
		shadowMap->bind(3);
		glUniform1i(usedShader->uniformLocation("shadowTexture"), 3);
	}

	mesh->render();

	usedShader->unbind();
	if (diffuseMap)
	{
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