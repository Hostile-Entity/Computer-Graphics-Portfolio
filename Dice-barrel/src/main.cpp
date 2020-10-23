#include "glToolbox.h"

//ShadowMap dimension
#define SHADOWMAP_WIDTH 2048
#define SHADOWMAP_HEIGHT 2048

GLFWwindow* window = NULL;

//Time variables
float delta_time, previous_time, current_time;

//ShadowMap variables
Camera shadowCamera, camera;
Framebuffer shadowMapFramebuffer;
Texture shadowMapTexture;
ShaderProgram shadowGenShaderProgram;

Mesh framebufferQuadMesh;

//Cinema stripes variables
Framebuffer cinemaStripesFramebuffer;
Texture cinemaStripesFramebufferDepthTexture;
Texture cinemaStripesFramebufferColorTexture;
Object cinemaStripesFramebufferQuad;
ShaderProgram cinemaStripesFramebufferShaderProgram;

//Object variables
Object skybox, dice, barrel, plane;
Mesh skyboxMesh, diceMesh, barrelMesh, planeMesh;
Texture skyboxCubemap, diceTexture, barrelTexture, planeTexture;
Texture diceNormalMap, barrelNormalMap, planeNormalMap;
Texture diceHeightMap, barrelHeightMap, planeHeightMap;
ShaderProgram skyboxShaderProgram, diceShaderProgram;

void initScene() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glDisable(GL_CULL_FACE);
	
	framebufferQuadMesh.load("assets/rect.obj");

	//Initializing Camera
	camera.pos = glm::vec3(4.0f, 2.5f, 6.0f);
	camera.rot = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	camera.distance = 15.0f;
	camera.pos = camera.distance * glm::normalize(camera.pos);

	//Initializing Shadow Map
	shadowCamera.pos = glm::vec3(6.0f, 10.0f, 6.0f);
	shadowCamera.rot = glm::quat(glm::lookAt(shadowCamera.pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
	shadowGenShaderProgram.loadShader("shaders/vertexShader.glsl", "shaders/fragGenShadows.glsl");
	shadowMapTexture.initDepth(SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
	shadowMapFramebuffer.initWithTextures(&shadowMapTexture);

	//Initializing Skybox
	skyboxMesh.load("assets/skybox.obj");
	skybox.mesh = &skyboxMesh;
	skyboxCubemap.loadTexture("assets/skybox.bmp");
	skybox.diffuseMap = &skyboxCubemap;
	skyboxShaderProgram.loadShader("shaders/vertexShader.glsl", "shaders/fragSkybox.glsl");
	skybox.shader = &skyboxShaderProgram;
	skybox.position = glm::vec3(0.0f, -20.0f, 0.0f);
	skybox.scale = 2.0f;

	//Initializing Dice
	diceMesh.load("assets/cube.obj");
	dice.mesh = &diceMesh;
	diceTexture.loadTexture("assets/cube.bmp");
	dice.diffuseMap = &diceTexture;
	diceNormalMap.loadTexture("assets/cubeNormal.bmp");
	dice.normalMap = &diceNormalMap;
	diceHeightMap.loadTexture("assets/cubeHeight.bmp");
	dice.heightMap = &diceHeightMap;
	dice.shadowMap = &shadowMapTexture;
	diceShaderProgram.loadShader("shaders/vertexShader.glsl", "shaders/fragShader.glsl");
	dice.shader = &diceShaderProgram;
	dice.position = glm::vec3(3.0f, 1.5f, 3.0f);
	dice.rotation = glm::quat(glm::radians(glm::vec3(5.0f, 30.0f, 0.0f)));
	dice.scale = 0.5f;

	//Initializing Barrel
	barrelMesh.load("assets/barrel.obj");
	barrel.mesh = &barrelMesh;
	barrelTexture.loadTexture("assets/barrel.bmp");
	barrel.diffuseMap = &barrelTexture;
	barrelNormalMap.loadTexture("assets/barrelNormal.png");
	barrel.normalMap = &barrelNormalMap;
	barrelHeightMap.loadTexture("assets/barrelHeight.bmp");
	barrel.heightMap = &barrelHeightMap;
	barrel.shadowMap = &shadowMapTexture;
	barrel.shader = &diceShaderProgram;
	barrel.position = glm::vec3(-2.0f,1.5f,-2.0f);
	barrel.scale = 2.0f;

	//Initializing Plane
	planeMesh.load("assets/plane.obj");
	plane.mesh = &planeMesh;
	planeTexture.loadTexture("assets/stones.jpg");
	plane.diffuseMap = &planeTexture;
	planeNormalMap.loadTexture("assets/stonesNormal.png");
	plane.normalMap = &planeNormalMap;
	planeHeightMap.loadTexture("assets/stonesHeight.png");
	plane.heightMap = &planeHeightMap;
	plane.shadowMap = &shadowMapTexture;
	plane.shader = &diceShaderProgram;
	plane.position = glm::vec3(0.0f, -1.0f, 0.0f);
	plane.scale = 0.4f;

	//Initializing Ciname Stripes Effect
	cinemaStripesFramebufferColorTexture.initColor();
	cinemaStripesFramebufferDepthTexture.initDepth();
	cinemaStripesFramebuffer.initWithTextures(&cinemaStripesFramebufferDepthTexture, &cinemaStripesFramebufferColorTexture);
	cinemaStripesFramebufferQuad.mesh = &framebufferQuadMesh;
	cinemaStripesFramebufferQuad.diffuseMap = &cinemaStripesFramebufferColorTexture;
	cinemaStripesFramebufferQuad.heightMap = &cinemaStripesFramebufferDepthTexture;
	cinemaStripesFramebufferShaderProgram.loadShader("shaders/vertexShaderFB.glsl", "shaders/fragCinemaStripesFB.glsl");
	cinemaStripesFramebufferQuad.shader = &cinemaStripesFramebufferShaderProgram;
}

void clear()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}

void render()
{
	//Time management
	current_time = glfwGetTime();
	delta_time = current_time - previous_time;
	previous_time = current_time;


	//Animation
	dice.rotation *= glm::quat(glm::radians(glm::vec3(0.0f, 10*delta_time, 0.0f)));

	//Rendering ShadowMap
	{
		shadowMapFramebuffer.bind();
		{
			glCullFace(GL_FRONT);
			glViewport(0, 0, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
			clear();
			skybox.draw(&shadowCamera, &shadowGenShaderProgram);
			dice.draw(&shadowCamera, &shadowGenShaderProgram);
			barrel.draw(&shadowCamera, &shadowGenShaderProgram);
			plane.draw(&shadowCamera, &shadowGenShaderProgram);
		}
		shadowMapFramebuffer.unbind();
	}
	//Rendering Scene
	{
		cinemaStripesFramebuffer.bind();
		{
			glCullFace(GL_BACK);
			glViewport(0, 0, WIDTH, HEIGHT);
			clear();
			skybox.draw(&camera, NULL, &shadowCamera);
			dice.draw(&camera, NULL, &shadowCamera);
			barrel.draw(&camera, NULL, &shadowCamera);
			plane.draw(&camera, NULL, &shadowCamera);
		}
		cinemaStripesFramebuffer.unbind();
		clear();
		cinemaStripesFramebufferQuad.draw(NULL);
	}

	//Keyboard
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.pos = glm::vec3(camera.pos.x * cos(-delta_time) - camera.pos.z * sin(-delta_time), camera.pos.y, camera.pos.z * cos(-delta_time) + camera.pos.x * sin(-delta_time));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.pos = glm::vec3(camera.pos.x * cos(delta_time) - camera.pos.z * sin(delta_time), camera.pos.y, camera.pos.z * cos(delta_time) + camera.pos.x * sin(delta_time));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.distance = camera.distance + 5 * delta_time;
		camera.pos = camera.distance * glm::normalize(camera.pos);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (camera.distance > 10 * delta_time) {
			camera.distance = camera.distance - 5 * delta_time;
			camera.pos = camera.distance * glm::normalize(camera.pos);
		}
	}
}

int main() {
	//Initializing window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Dice-Barrel", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

#if _WIN32
	if (WGLEW_EXT_swap_control) {
		wglSwapIntervalEXT(1);
	}
#endif
	//Initializing Scene
	initScene();
	previous_time = glfwGetTime();

	//Running program
	do {
		glfwPollEvents();
		render();
		glfwSwapBuffers(window);
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwTerminate();
	return 0;
}

