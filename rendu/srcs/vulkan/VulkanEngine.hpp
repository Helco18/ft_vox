#pragma once

#define VK_USE_PLATFORM_XCB_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <iostream>
#include <iterator>
#include <optional>
#include "colors.hpp"

#define VULKAN_CALLBACK VKAPI_ATTR vk::Bool32 VKAPI_CALL
#define DEBUG_LEVEL vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose

const std::vector g_validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

// La swapchain servira à présenter des images à la fenêtre
// Les autres ajoutent des fonctionnalités supplémentaires (?)
const std::vector<const char*> g_deviceExtensions =
{
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};

constexpr bool g_enableValidationLayers = true;
// #ifdef DEBUG
// constexpr bool g_enableValidationLayers = true;
// #else
// constexpr bool g_enableValidationLayers = false;
// #endif

struct QueueIndices
{
	std::optional<uint32_t>	graphicsIndex;
	std::optional<uint32_t>	presentIndex;
};

class VulkanEngine
{
	public:
		VulkanEngine(GLFWwindow * window);
		~VulkanEngine();

	private:
		GLFWwindow *						_window;
		vk::raii::Context					_context;
		vk::raii::Instance					_instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT	_debugMessenger = nullptr;
		vk::raii::PhysicalDevice			_physicalDevice = nullptr;
		vk::raii::Device					_device = nullptr;
		vk::raii::Queue						_graphicsQueue = nullptr;
		vk::raii::Queue						_presentQueue = nullptr;
		vk::raii::SurfaceKHR				_surface = nullptr;

		typedef std::vector<char const *>	RequiredExtensions;
		typedef std::vector<char const *>	RequiredLayers;

		void								_createInstance();
		void								_initDebugMessenger();
		void								_createSurface();
		RequiredExtensions					_getRequiredExtensions() const;
		RequiredLayers						_getRequiredLayers() const;
		void								_selectPhysicalDevice();
		QueueIndices						_findQueueFamilies() const;
		void								_createLogicalDevice();
};
