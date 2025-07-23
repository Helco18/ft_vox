/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanEngine.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 15:07:42 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/23 19:19:18 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VulkanEngine.hpp"
#include "tools/utils.hpp"

VulkanEngine::VulkanEngine(GLFWwindow *window) : _window(window)
{}

VulkanEngine::~VulkanEngine()
{}

void VulkanEngine::createInstance()
{
	// App informations
	VkApplicationInfo appInfo{};
	// Set the application name, version, engine name, and API version
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ft_vox";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Very Real Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

	// Validation layers
	_validationLayers = {"VK_LAYER_KHRONOS_validation"};

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = _validationLayers.size();
	createInfo.ppEnabledLayerNames = _validationLayers.data();

	// Extensions
	// Get the required extensions for GLFW and add them to the create info
	// This is necessary for Vulkan to work with GLFW
	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	# ifdef DEBUG
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	// Debug pour log les erreurs
	populateDebugMessengerCreateInfo(debugCreateInfo);
	createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	# else
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;
	# endif

	// Créer l'instance
	// vkCreateInstance will return VK_SUCCESS if the instance was created successfully
	VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create instance! Code: " + toString(result));
}

void VulkanEngine::createLogicalDevice()
{
	// Find the queue families for the physical device
	_indices = findQueueFamilies(_physicalDevice);

	// Check if the graphics family is available
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = _indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	// Set the queue priority to 1.0f (highest priority)
	// This is necessary for the queue to be created successfully
	// If the queue priority is not set, the queue will not be created
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	// Create the logical device
	// This will create a logical device that can be used to interact with the physical device
	// The logical device will have the queue family that was created earlier
	// The device features are set to default, but can be modified if needed
	VkPhysicalDeviceFeatures deviceFeatures{};
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;

	// If DEBUG is true, enable the validation layers
	# ifdef DEBUG
	{
	createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
	createInfo.ppEnabledLayerNames = _validationLayers.data();
	}
	# else
	createInfo.enabledLayerCount = 0;
	# endif
	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
    	throw std::runtime_error("Failed to create logical device!");
	// Get the graphics queue from the logical device
	// This will return the queue that was created earlier
	// The queue will be used to submit commands to the physical device
	vkGetDeviceQueue(_device, _indices.graphicsFamily.value(), 0, &_graphicsQueue);
}

void VulkanEngine::pickGraphicsCard()
{
	// This function will pick the first suitable physical device that supports Vulkan
	// It will check if the device is suitable by calling isDeviceSuitable
	// If a suitable device is found, it will be set as the _physicalDevice
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	// Compte le nombre de GPU
	uint32_t deviceCount = 0;

	vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
	if (deviceCount == 0)
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");

	// Si le nombre de GPU est supérieur à 0, on les récupère
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
	std::cout << GREEN << "JAI TROUVER " << deviceCount << " GPUs" << RESET << std::endl;
	// On parcourt les GPU et on vérifie s'ils sont compatibles avec Vulkan
	// Si un GPU est compatible, on le set comme _physicalDevice
	// Si aucun GPU n'est compatible, on lance une erreur
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
