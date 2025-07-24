/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vulkan_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 16:40:28 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/24 19:17:55 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vulkan/VulkanEngine.hpp"
#include <algorithm>
#include <limits>
#include <vulkan/vulkan_core.h>

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice & device, const VkSurfaceKHR & surface)
{
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	int	i = 0;
	VkBool32 presentSupport = false;

	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
	for (const auto & queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;
		if (presentSupport)
			indices.presentFamily = i;
		i++;
	}
	return indices;
}

SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice & device, const VkSurfaceKHR & surface)
{
	SwapChainSupportDetails details;
	uint32_t formatCount;
	uint32_t presentModeCount;
	
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}

static bool checkDeviceExtensionSupport(const VkPhysicalDevice & device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}
	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities, GLFWwindow * window)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;
	else
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

bool isDeviceSuitable(const VkPhysicalDevice & device, const QueueFamilyIndices & indices, const VkSurfaceKHR & surface)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;

	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	// Discrete GPU = la bonne !! :) (celle qui n'est pas intégrée ect.)
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& deviceFeatures.geometryShader
		&& indices.isComplete()
		&& checkDeviceExtensionSupport(device))
	{
		std::cout << CYAN << "GPU: " << deviceProperties.deviceName << RESET << std::endl;
		bool swapChainAdequate = false;
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		return swapChainAdequate;
	}
	return false;
}

std::vector<const char * > getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char ** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char * > extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	# ifdef DEBUG
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	# endif

	return extensions;
}

# ifdef DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT              messageType,
	const VkDebugUtilsMessengerCallbackDataEXT*  pCallbackData,
	void*                                        pUserData) {
	
	if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		return VK_TRUE;
	std::string prefix = messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT ? BLACK "[INFO] " : 
						 messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT ? YELLOW "[WARNING] " : RED "[ERROR] " ;
	(void) messageType;
	(void) pUserData;

	std::cerr << prefix << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	createInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr; // optionnel
}
# endif
