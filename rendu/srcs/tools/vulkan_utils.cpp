/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vulkan_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 16:40:28 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/24 18:40:10 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vulkan/VulkanEngine.hpp"
#include <vulkan/vulkan_core.h>

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice & device, const VkSurfaceKHR & surface) {
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

bool isDeviceSuitable(const VkPhysicalDevice & device, const QueueFamilyIndices & indices)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;

	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	// Discrete GPU = la bonne !! :) (celle qui n'est pas intégrée ect.)
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& deviceFeatures.geometryShader
		&& indices.isComplete())
	{
		std::cout << CYAN << "GPU: " << deviceProperties.deviceName << RESET << std::endl;
		return true;
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
