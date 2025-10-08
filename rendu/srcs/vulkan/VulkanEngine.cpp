#include "VulkanEngine.hpp"

VulkanEngine::VulkanEngine(GLFWwindow * window): _window(window)
{
	_createInstance();
	_initDebugMessenger();
	_createSurface();
	_selectPhysicalDevice();
	_checkDeviceExtensions();
	_createLogicalDevice();
	_createSwapChain();
	_createImageViews();
}

VulkanEngine::~VulkanEngine()
{
}
