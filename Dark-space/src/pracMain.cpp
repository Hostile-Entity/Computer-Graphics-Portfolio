#include "include/scene.hpp"

GLFWwindow* window = NULL;

Scene mainScene;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dark-space", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	mainScene.window = window;
	mainScene.init();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		mainScene.render();
		mainScene.interface();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}