#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <chrono>
#include <iostream>
#include "Camera.h"
#include "Scene.h"

int Scene::FLAG = 0; // <---------------------- Change this to switch between bunny and default scene
int Scene::SAMPLING = 6; // < ----------------- This is equal to the sqrt(# samples per pixel), e.g. 7 --> 49 viewing rays traced per pixel 
int Camera::MAX_RECURSION_DEPTH = 4; // <------ This is the number of recursive calls/traced reflection rays

#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 1200

float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
GLFWwindow *window;

Scene* scene = new Scene();
Camera camera = Camera(WINDOW_WIDTH,
											 WINDOW_HEIGHT,
											 1.0f,
											 glm::radians(45.0f),
											 glm::vec3(0.0f, 0.0f, 7.0f),
											 glm::vec3(0.0f, 0.0f, 0.0f),
											 glm::vec3(0.0f, 1.0f, 0.0f));


void ClearFrameBuffer() {
	memset(&frameBuffer, 0, WINDOW_HEIGHT * WINDOW_WIDTH * 3 * sizeof(float));
}

void Display() {
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, frameBuffer);
}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment5 - Sachin Chanchani", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ClearFrameBuffer();

	scene->LoadScene();

	auto start = std::chrono::high_resolution_clock::now();
	camera.TakePicture(scene);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "Time passed (ms): " << duration.count() << std::endl;

	float *renderedImage = camera.GetRenderedImage();
	memcpy(frameBuffer, renderedImage, sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
}


int main()
{
	Init();
	while ( glfwWindowShouldClose(window) == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
