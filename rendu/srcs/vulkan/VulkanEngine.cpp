#include "VulkanEngine.hpp"

VulkanEngine::VulkanEngine(GLFWwindow * window): _window(window)
{
	_createInstance();
	_initDebugMessenger();
	_createSurface();
	_selectPhysicalDevice();
	_createLogicalDevice();
}

VulkanEngine::~VulkanEngine()
{
	glfwDestroyWindow(_window);
	glfwTerminate();
}
