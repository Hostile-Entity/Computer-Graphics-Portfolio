#pragma once

#include "include/lib.h"

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 right;
	glm::vec3 up;

	glm::mat4 viewMatrix() const
	{
		glm::mat4 view = glm::lookAt(
			position,
			position + direction,
			glm::vec3(0,1,0)
		);
		return view;
	}
	glm::mat4 projectionMatrix() const
	{
		glm::mat4 projection = glm::perspective(3.14f / 3.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		return projection;
	}
};