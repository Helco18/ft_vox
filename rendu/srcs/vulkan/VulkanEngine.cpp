/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanEngine.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 15:07:42 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/24 19:45:54 by scraeyme         ###   ########.fr       */
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
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {_indices.graphicsFamily.value(), _indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Create the logical device
	// This will create a logical device that can be used to interact with the physical device
	// The logical device will have the queue family that was created earlier
	// The device features are set to default, but can be modified if needed
	VkPhysicalDeviceFeatures deviceFeatures{};
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

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
	vkGetDeviceQueue(_device, _indices.presentFamily.value(), 0, &_presentQueue);
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
		_indices = findQueueFamilies(device, _surface);
		if (isDeviceSuitable(device, _indices, _surface))
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

void VulkanEngine::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice, _surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, _window);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = _surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = {_indices.graphicsFamily.value(), _indices.presentFamily.value()};

	if (_indices.graphicsFamily != _indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE; // hehe coucou moi du futur :) j'espere t'as un triangle

	if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
    	throw std::runtime_error("Failed to create swap chain!");
	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
	_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());
	_swapChainImageFormat = surfaceFormat.format;
	_swapChainExtent = extent;
}

void VulkanEngine::createImageViews()
{
    _swapChainImageViews.resize(_swapChainImages.size());
	for (size_t i = 0; i < _swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = _swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS)
    		throw std::runtime_error("Failed to create image views!");
	}
}

void VulkanEngine::createGraphicsPipeline()
{

}

void VulkanEngine::init()
{
	createInstance();
	createSurface();
	pickGraphicsCard();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createGraphicsPipeline();
}

void VulkanEngine::destroy()
{
	for (auto imageView : _swapChainImageViews)
        vkDestroyImageView(_device, imageView, nullptr);
	vkDestroySwapchainKHR(_device, _swapChain, nullptr);
	vkDestroyDevice(_device, nullptr);
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyInstance(_instance, nullptr);
	std::cout << GREEN << "Vulkan instance destroyed" << RESET << std::endl;
}
