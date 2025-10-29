#include "stb/stb_image.h"
#include "WindowManager.hpp"

void toggleFullscreen(GLFWwindow * window, Camera * camera)
{
	static int windowPosX = 0;
	static int windowPosY = 0;
	static int windowWidth = 0;
	static int windowHeight = 0;
	static bool isFullscreen = false;

	if (!isFullscreen)
	{
		glfwGetWindowPos(window, &windowPosX, &windowPosY);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);

		GLFWmonitor * monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode * mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		camera->setWidth(mode->width);
		camera->setHeight(mode->height);
		isFullscreen = true;
	}
	else
	{
		glfwSetWindowMonitor(window, nullptr, windowPosX, windowPosY, windowWidth, windowHeight, 0);
		camera->setWidth(windowWidth);
		camera->setHeight(windowHeight);
		isFullscreen = false;
	}
}

void framebufferResizeCallback(GLFWwindow * window, int width, int height)
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));
	AEngine * engine = windowManager->getEngine();
	Camera * camera = engine->getCamera();
	camera->setWidth(width);
	camera->setHeight(height);
	engine->setFramebufferResized(true);
}
