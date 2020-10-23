#include "include/scene.hpp"

void Scene::init()
{
	time = glfwGetTime();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glDisable(GL_CULL_FACE);

	camera.position = glm::vec3(0.0f, 1.0f, 3.0f);
	camera.direction = glm::vec3(0, 0, -1);

	framebufferQuadMesh.load("obj/rectangle.obj");

	shadowGenShaderProgram.loadShader("glsl/shader.vert", "glsl/gen_shadows.frag");
	shadowMapTexture.initDepth(SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
	shadowMapFramebuffer.initWithTextures(&shadowMapTexture);
	shadowCamera.position = glm::vec3(6.0f, 6.0f, 6.0f);
	shadowCamera.direction = glm::vec3(-1, -1, -1);

	skyboxMesh.load("obj/cube.obj");
	skybox.mesh = &skyboxMesh;
	skyboxCubemap.loadCubeTexture("texture/space", ".jpg");
	skybox.diffuseMap = &skyboxCubemap;
	skyboxShaderProgram.loadShader("glsl/shader.vert", "glsl/skybox.frag");
	skybox.shader = &skyboxShaderProgram;
	skybox.scale = 70.0;

	earthMesh.load("obj/ball.obj");
	earth.mesh = &earthMesh;
	earthTexture.loadTexture("texture/earth.jpg");
	earth.diffuseMap = &earthTexture;
	earthNormalMap.loadTexture("texture/earthNormal.png");
	earth.normalMap = &earthNormalMap;
	earthHeightMap.loadTexture("texture/earthHeight.png");
	earth.heightMap = &earthHeightMap;
	earth.shadowMap = &shadowMapTexture;
	earthShaderProgram.loadShader("glsl/shader.vert", "glsl/shader.frag");
	earth.shader = &earthShaderProgram;
	earth.position = glm::vec3(0.0f, 0.0f, 0.0f);
	earth.rotation = glm::quat(glm::radians(glm::vec3(0.0f, 23.5f, 0.0f)));

	asteroidMesh.load("obj/asteroid.obj");
	asteroid.mesh = &asteroidMesh;
	asteroidTexture.loadTexture("texture/asteroid.jpg");
	asteroid.diffuseMap = &asteroidTexture;
	asteroidNormalMap.loadTexture("texture/asteroidNormal.png");
	asteroid.normalMap = &asteroidNormalMap;
	asteroid.shadowMap = &shadowMapTexture;
	asteroid.shader = &earthShaderProgram;
	asteroid.position = glm::vec3(1, 1, 1);
	asteroid.scale = 0.01f;

	spaceshipMesh.load("obj/spaceship.obj");
	spaceship.mesh = &spaceshipMesh;
	spaceship.diffuseMap = &skyboxCubemap;
	spaceship.shadowMap = &shadowMapTexture;
	spaceshipShaderProgram.loadShader("glsl/shader.vert", "glsl/refraction.frag");
	spaceship.shader = &spaceshipShaderProgram;
	spaceship.position = glm::vec3(0,1,-4);
	spaceship.rotation = glm::quat(glm::radians(glm::vec3(0.0f, -90.0f, 0.0f)));
	spaceship.scale = 0.3f;
	spaceship.speed = 1.0f;
}

void Scene::render()
{
	delta = glfwGetTime() - time;
	time += delta;
	earth.rotation *= glm::quat(glm::radians(glm::vec3(0.0f, 15.0f*delta, 0.0f)));
	spaceship.position = glm::vec3(spaceship.position.x * cos(-spaceship.speed *delta) - spaceship.position.z * sin(-spaceship.speed * delta), spaceship.position.y, spaceship.position.z * cos(-spaceship.speed * delta) + spaceship.position.x * sin(-spaceship.speed *  delta));
	spaceship.rotation *= glm::quat(glm::vec3(0.0f, spaceship.speed * delta, 0.0f));
	asteroid.rotation *= glm::quat(glm::radians(glm::vec3(0.0f, 15.0f * delta, 0.0f)));

	shadowMapFramebuffer.bind();

	glCullFace(GL_FRONT);
	glViewport(0, 0, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
	clear();
	skybox.draw(&shadowCamera, &shadowGenShaderProgram);
	earth.draw(&shadowCamera, &shadowGenShaderProgram);
	spaceship.draw(&shadowCamera, &shadowGenShaderProgram);
	asteroid.draw(&shadowCamera, &shadowGenShaderProgram);

	shadowMapFramebuffer.unbind();

	glCullFace(GL_BACK);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	clear();
	skybox.draw(&camera, NULL, &shadowCamera);
	earth.draw(&camera, NULL, &shadowCamera);
	spaceship.draw(&camera, NULL, &shadowCamera);
	asteroid.draw(&camera, NULL, &shadowCamera);
}

void Scene::interface()
{
	float speed = 1.0f;

	camera.right = glm::normalize(glm::cross(glm::vec3(camera.direction), glm::vec3(0, 1, 0)));
	camera.up = glm::normalize(glm::cross(camera.right, glm::vec3(camera.direction)));
	//turn right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.direction = glm::normalize(glm::rotate(camera.direction, -speed * delta, camera.up));
	}
	//turn left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.direction = glm::normalize(glm::rotate(camera.direction, speed * delta, camera.up));
	}
	//turn up
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.direction = glm::normalize(glm::rotate(camera.direction, speed * delta, camera.right));
	}
	//turn down
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.direction = glm::normalize(glm::rotate(camera.direction, -speed * delta, camera.right));
	}
}