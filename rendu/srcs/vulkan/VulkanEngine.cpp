#include "VulkanEngine.hpp"

VulkanEngine::VulkanEngine()
{
	_initWindow();
	_initVulkan();
	_selectPhysicalDevice();
}

VulkanEngine::~VulkanEngine()
{
	glfwDestroyWindow(_window);
	glfwTerminate();
}

/**
 * Initializes GLFW window
 * 
 */
void VulkanEngine::_initWindow()
{
	glfwInit();

	// Vu que GLFW créé un contexte OpenGL par défaut, on lui précise de ne pas le faire.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// On va gérer nous même le resize de la fenêtre
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Width, height, titre de la fenêtre, écran sur lequel afficher (GLFWMonitor, nullptr pour défaut)
	// Le dernier paramètre est pour OpenGL, on met donc nullptr.
	_window = glfwCreateWindow(WIDTH, HEIGHT, "scop", nullptr, nullptr);
}

void VulkanEngine::_initVulkan()
{
	_createInstance();
	_initDebugMessenger();
}

void VulkanEngine::loop()
{
	while (!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();
	}
}
