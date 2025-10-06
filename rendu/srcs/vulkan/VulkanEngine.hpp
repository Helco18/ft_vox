#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <iostream>
#include "colors.hpp"

#define WIDTH 1280
#define HEIGHT 720
#define VULKAN_CALLBACK VKAPI_ATTR vk::Bool32 VKAPI_CALL
#define DEBUG_LEVEL vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose

const std::vector g_validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

constexpr bool g_enableValidationLayers = true;
// #ifdef DEBUG
// constexpr bool g_enableValidationLayers = true;
// #else
// constexpr bool g_enableValidationLayers = false;
// #endif

class VulkanEngine
{
	public:
		VulkanEngine();
		~VulkanEngine();

		void								loop();

	private:
		GLFWwindow *						_window;
		vk::raii::Context					_context;
		vk::raii::Instance					_instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT	_debugMessenger = nullptr;
		vk::raii::PhysicalDevice			_physicalDevice = nullptr;

		typedef std::vector<char const *>	RequiredExtensions;
		typedef std::vector<char const *>	RequiredLayers;

		void								_initWindow();
		void								_initVulkan();
		void								_initDebugMessenger();
		void								_createInstance();
		RequiredExtensions					_getRequiredExtensions() const;
		RequiredLayers						_getRequiredLayers() const;
		void								_selectPhysicalDevice();
};
