/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanEngine.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 15:07:42 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/22 19:22:20 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VulkanEngine.hpp"
#include "tools/utils.hpp"

VulkanEngine::VulkanEngine(GLFWwindow *window) : _window(window)
{
	
}

VulkanEngine::~VulkanEngine()
{
	
}

void VulkanEngine::createInstance()
{
	// App informations
	VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ft_vox";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Very Real Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

	// Send instance info ?
	_validationLayers = {
    	"VK_LAYER_KHRONOS_validation"
	};
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = _validationLayers.size();
	createInfo.ppEnabledLayerNames = _validationLayers.data();

	// Extensions ?
	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	// Debug pour log les erreurs
    if (IS_DEBUG) {
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

	// Créer l'instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create instance! Code: " + toString(result));
}

void VulkanEngine::createLogicalDevice()
{
	_indices = findQueueFamilies(_physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = _indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;

	if (IS_DEBUG)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
		createInfo.ppEnabledLayerNames = _validationLayers.data();
	}
	else
		createInfo.enabledLayerCount = 0;
	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
    	throw std::runtime_error("Failed to create logical device!");
	vkGetDeviceQueue(_device, _indices.graphicsFamily.value(), 0, &_graphicsQueue);
}

void VulkanEngine::pickGraphicsCard()
{
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	// Compte le nombre de GPU
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
	if (deviceCount == 0)
    	throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
	std::cout << GREEN << "JAI TROUVER " << deviceCount << " GPUs PUTAIN" << RESET << std::endl;
	for (const auto &device : devices)
	{
		if (isDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	_physicalDevice = physicalDevice;
}

void VulkanEngine::createSurface()
{
	if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");
}

void VulkanEngine::init()
{
	createInstance();
	pickGraphicsCard();
	createLogicalDevice();
	createSurface();
}

void VulkanEngine::destroy()
{
	vkDestroyDevice(_device, nullptr);
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyInstance(_instance, nullptr);
	std::cout << GREEN << "Vulkan instance destroyed" << RESET << std::endl;
}
