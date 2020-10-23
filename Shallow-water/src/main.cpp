//internal includes
#include "common.h"
#include "extra.hpp"
#include "ShaderProgram.h"

//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

using namespace glm;

static const GLsizei WIDTH = 1280, HEIGHT = 960; //размеры окна
static const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

const int NUMBER_OF_OBJECTS = 5;
const int OBJECTS_ARRAY[NUMBER_OF_OBJECTS] = { 1, 1, 200, 1, 1 };
const int MAX_NUMBER_OB_OBJECTS = 200;



std::random_device rnd_dev;
std::mt19937 gen(rnd_dev());
std::uniform_real_distribution<> r_distr(0, 1);

// Generates random float
inline float randomFloat(
	float from = 0.0f,
	float to = 1.0f)
{
	return r_distr(gen) * (to - from) + from;
}

int initGL()
{
	int res = 0;
	//грузим функции opengl через glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: "   << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: "  << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: "     << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	return 0;
}

int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

	//запрашиваем контекст opengl версии 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

	GLFWwindow*  window = glfwCreateWindow(WIDTH, HEIGHT, "Shallow-water", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	//создание шейдерной программы из двух файлов с исходниками шейдеров
	//используется класс-обертка ShaderProgram
	std::unordered_map<GLenum, std::string> shaders;
	shaders[GL_VERTEX_SHADER]   = "vertex.glsl";
	shaders[GL_FRAGMENT_SHADER] = "fragment.glsl";
	ShaderProgram program(shaders); GL_CHECK_ERRORS;

	GLuint MaterialID = glGetUniformLocation(program.GetProgram(), "material");
	GLuint TextureID = glGetUniformLocation(program.GetProgram(), "textureID");
	GLuint MatrixID = glGetUniformLocation(program.GetProgram(), "MVP");
	GLuint ViewID = glGetUniformLocation(program.GetProgram(), "ViewID");
	GLuint ModelID = glGetUniformLocation(program.GetProgram(), "ModelID");

	glfwSwapInterval(1); // force 60 frames per second

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	const char* paths[NUMBER_OF_OBJECTS];
	std::vector<vec3> vertices[NUMBER_OF_OBJECTS];
	std::vector<vec2> uvs[NUMBER_OF_OBJECTS];
	std::vector<vec3> normals[NUMBER_OF_OBJECTS];
	std::vector<unsigned short> indices[NUMBER_OF_OBJECTS];
	std::vector<vec3> indexed_vertices[NUMBER_OF_OBJECTS];
	std::vector<vec2> indexed_uvs[NUMBER_OF_OBJECTS];
	std::vector<vec3> indexed_normals[NUMBER_OF_OBJECTS];
	GLuint Texture[NUMBER_OF_OBJECTS];
	int material[NUMBER_OF_OBJECTS];
	GLuint vertexbuffer[NUMBER_OF_OBJECTS];
	GLuint uvbuffer[NUMBER_OF_OBJECTS];
	GLuint normalbuffer[NUMBER_OF_OBJECTS];
	GLuint elementbuffer[NUMBER_OF_OBJECTS];

	paths[0] = "box.obj";
	Texture[0] = loadBMP_custom("box.bmp");
	paths[1] = "fish.obj";
	Texture[1] = loadBMP_custom("fishTexture.bmp");
	paths[2] = "seaweed.obj";
	Texture[2] = loadBMP_custom("seaweedTexture.bmp");
	paths[3] = "skybox.obj";
	Texture[3] = loadBMP_custom("skybox.bmp");
	paths[NUMBER_OF_OBJECTS - 1] = "water.obj";
	Texture[NUMBER_OF_OBJECTS - 1] = loadBMP_custom("waterTexture.bmp");

	for (int i = 0; i < NUMBER_OF_OBJECTS; i++) {
		loadOBJ(paths[i], vertices[i], uvs[i], normals[i]);
		indexVBO(vertices[i], uvs[i], normals[i], indices[i], indexed_vertices[i], indexed_uvs[i], indexed_normals[i]);

		glGenBuffers(1, &vertexbuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, indexed_vertices[i].size() * sizeof(glm::vec3), &indexed_vertices[i][0], GL_STATIC_DRAW);

		glGenBuffers(1, &uvbuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, indexed_uvs[i].size() * sizeof(glm::vec2), &indexed_uvs[i][0], GL_STATIC_DRAW);

		glGenBuffers(1, &normalbuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, indexed_normals[i].size() * sizeof(glm::vec3), &indexed_normals[i][0], GL_STATIC_DRAW);

		glGenBuffers(1, &elementbuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(unsigned short), &indices[i][0], GL_STATIC_DRAW);
		
		material[i] = 0;
	}

	//water
	std::vector<vec3> A = indexed_vertices[NUMBER_OF_OBJECTS - 1], B = A;
	const float CENTER_COORD = WATER_WIDTH / 2 * (WATER_WIDTH + 1);

	//Cam
	glm::vec3 cameraPosBasic = glm::vec3(3, 1, 3);
	glm::vec3 cameraPosition = cameraPosBasic;
	float horizontalAngle = 3.95f;
	float verticalAngle = 5.99f;
	const float mouseSpeed = 0.0005f;
	const float speedMove = 3.0f;
	bool PAUSE = false;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mat4 MVP[NUMBER_OF_OBJECTS][MAX_NUMBER_OB_OBJECTS];

	vec3 o_position[NUMBER_OF_OBJECTS][MAX_NUMBER_OB_OBJECTS];
	vec3 o_direction[NUMBER_OF_OBJECTS][MAX_NUMBER_OB_OBJECTS];
	quat o_rotation[NUMBER_OF_OBJECTS][MAX_NUMBER_OB_OBJECTS];
	vec3 o_scale[NUMBER_OF_OBJECTS][MAX_NUMBER_OB_OBJECTS];

	for (int i = 0; i < NUMBER_OF_OBJECTS; ++i)
		for (int j = 0; j < OBJECTS_ARRAY[i]; ++j) {
			o_position[i][j] = vec3(0, 0, 0);
			o_direction[i][j] = vec3(0, 0, 0);
			o_rotation[i][j] = quat(0, 0, 0, 0);
			o_scale[i][j] = vec3(1, 1, 1);
		}

	//SETTING THE SCENE
	o_position[0][0] = vec3(0, -0.3, 0);
	o_scale[0][0] = vec3(2, 1, 2);

	o_position[1][0] = vec3(0, -0.3, 1);
	o_scale[1][0] = vec3(0.02, 0.02, 0.02);
	float fishSpeed = 1.0f;
	o_rotation[1][0] = glm::quat(glm::radians(glm::vec3(0.0f, 180.0f, 0.0f)));

	for (int j = 0; j < OBJECTS_ARRAY[2]; ++j) {
		o_position[2][j] = vec3(randomFloat(-2 + 0.2, 2 - 0.2), -0.7, randomFloat(-2 + 0.2, 2 - 0.2));
		o_scale[2][j] = vec3(0.05 + randomFloat(-0.02, 0.02), 0.02 + randomFloat(-0.005, 0.01), 0.05 + randomFloat(-0.02, 0.02));
	}

	o_position[3][0] = vec3(0, -5, 0);
	material[3] = 2;

	material[NUMBER_OF_OBJECTS - 1] = 1;
	o_scale[NUMBER_OF_OBJECTS - 1][0] = vec3(2, 1, 2);


	//
	double time0 = glfwGetTime();
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//цикл обработки сообщений и отрисовки сцены каждый кадр
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glfwPollEvents();

		double time1 = glfwGetTime();
		float delta = time1 - time0;
		time0 = time1;
		


		//camera
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// Reset mouse position for next frame
		glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

		horizontalAngle += mouseSpeed * float(WIDTH / 2 - xpos);
		verticalAngle += mouseSpeed * float(HEIGHT / 2 - ypos);

		glm::vec3 direction(
			cos(verticalAngle)* sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle)* cos(horizontalAngle)
		);

		// Right vector
		glm::vec3 right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f)
		);
		glm::vec3 up = glm::cross(right, direction);
		//std::cout << horizontalAngle << " " << verticalAngle << std::endl;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			cameraPosBasic += speedMove * delta * direction;
		}
		//back
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			cameraPosBasic -= speedMove * delta * direction;
		}
		//right
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			cameraPosBasic += speedMove * delta * right;
		}
		//left
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			cameraPosBasic -= speedMove * delta * right;
		}
		cameraPosition = cameraPosBasic;





		//simulate
		if (!PAUSE) {
			simulation(A, B, indexed_vertices[NUMBER_OF_OBJECTS - 1]);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[NUMBER_OF_OBJECTS - 1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, indexed_vertices[NUMBER_OF_OBJECTS - 1].size() * sizeof(glm::vec3), &indexed_vertices[NUMBER_OF_OBJECTS - 1][0]);

			newNormals(indexed_vertices[NUMBER_OF_OBJECTS - 1], indexed_normals[NUMBER_OF_OBJECTS - 1]);
			glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[NUMBER_OF_OBJECTS - 1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, indexed_normals[NUMBER_OF_OBJECTS - 1].size() * sizeof(vec3), &indexed_normals[NUMBER_OF_OBJECTS - 1][0]);
		}
		//Matrix
		glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + direction, up);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), ASPECT_RATIO, 0.1f, 100.0f);

		//fish movement
		if (!PAUSE) {
			o_position[1][0] = glm::vec3(o_position[1][0].x * cos(-fishSpeed * delta) - o_position[1][0].z * sin(-fishSpeed * delta), o_position[1][0].y, o_position[1][0].z * cos(-fishSpeed * delta) + o_position[1][0].x * sin(-fishSpeed * delta));
			o_rotation[1][0] *= glm::quat(glm::vec3(0.0f, fishSpeed * delta, 0.0f));
		}

		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		program.StartUseShader();
		for (int i = 0; i < NUMBER_OF_OBJECTS; ++i){
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if (i == NUMBER_OF_OBJECTS - 1 && glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture[i]);
			glUniform1i(TextureID, 0);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[i]);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[i]);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[i]);

			for (int j = 0; j < OBJECTS_ARRAY[i]; ++j) {
				glm::mat4 ModelMat = mat4(1.0);
				ModelMat = glm::translate(ModelMat, o_position[i][j]) * glm::toMat4(o_rotation[i][j]);
				ModelMat = glm::scale(ModelMat, o_scale[i][j]);
				MVP[i][j] = projection * view * ModelMat;

				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[i][j][0][0]);
				glUniformMatrix4fv(ViewID, 1, GL_FALSE, &view[0][0]);
				glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelMat[0][0]);
				glUniform1i(MaterialID, material[i]);
				glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_SHORT, (void*)0);
			}
		}
    

		program.StopUseShader();

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
			indexed_vertices[NUMBER_OF_OBJECTS - 1][CENTER_COORD].y = 0.6;
			indexed_vertices[NUMBER_OF_OBJECTS - 1][CENTER_COORD + 1].y = 0.6;
			indexed_vertices[NUMBER_OF_OBJECTS - 1][CENTER_COORD - 1].y = 0.6;
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[NUMBER_OF_OBJECTS - 1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, indexed_vertices[NUMBER_OF_OBJECTS - 1].size() * sizeof(glm::vec3), &indexed_vertices[NUMBER_OF_OBJECTS-1][0]);
		}
		if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
			PAUSE = true;
		} else{
			PAUSE = false;
		}

		//Обновление вершин водной поверхности
		if (!PAUSE) {
			rotateVectors(A, B, indexed_vertices[NUMBER_OF_OBJECTS - 1]);
		}

		glfwSwapBuffers(window); 
	}

	glfwTerminate();
	return 0;
}
