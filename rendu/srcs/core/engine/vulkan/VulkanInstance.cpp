#include "VulkanEngine.hpp"
#include "colors.hpp"
#include <iostream>
#include <map>

void VulkanEngine::_createInstance()
{
	// On donne les informations de notre application à Vulkan
	vk::ApplicationInfo appInfo;
	appInfo.pApplicationName = "scop";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Very Real Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = vk::ApiVersion14;

	// On récupère les validation layers requis par notre implémentation Vulkan
	RequiredLayers requiredLayers = _getRequiredLayers();

	// On récupère les informations concernant les extensions Vulkan dont GLFW a besoin
	RequiredExtensions requiredExtensions = _getRequiredExtensions();

	// Puis on les stocke dans Vulkan
	vk::InstanceCreateInfo createInfo;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
	createInfo.ppEnabledLayerNames = requiredLayers.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	// On créé ensuite notre instance Vulkan (On passe par RAII pour qu'elle soit détruite automatiquement)
	_instance = vk::raii::Instance(_context, createInfo);

	if (g_enableValidationLayers)
		std::cout << GREEN << "[OK] Created Instance" << RESET << std::endl;
}

VulkanEngine::RequiredLayers VulkanEngine::_getRequiredLayers() const
{
	RequiredLayers requiredLayers;
	if (g_enableValidationLayers)
		requiredLayers.assign(g_validationLayers.begin(), g_validationLayers.end());

	// On regarde si les layers requis sont supportés par notre implémentation Vulkan
	std::vector<vk::LayerProperties> layerProperties = _context.enumerateInstanceLayerProperties();
	for (uint32_t i = 0; i < requiredLayers.size(); ++i)
	{
		bool foundExtension = false;
		const char * tmp = requiredLayers[i];
		for (vk::LayerProperties layerProperty : layerProperties)
		{
			if (strcmp(layerProperty.layerName, tmp) == 0)
			{
				foundExtension = true;
				break;
			}
		}
		if (!foundExtension)
			throw std::runtime_error("Required validation layer not supported: " + std::string(tmp));
	}
	return requiredLayers;
}

/**
 * Vérifie que toutes les extensions Vulkan requises par GLFW sont supportées par le système.
 * Si une extension manque, la création de l’instance Vulkan pourrait échouer, empêchant GLFW de fonctionner correctement avec Vulkan.
 */
VulkanEngine::RequiredExtensions VulkanEngine::_getRequiredExtensions() const
{
	RequiredExtensions requiredExtensions;
	uint32_t extensionCount = 0;

	// On récupère le nombre et le nom des extensions dont GLFW a besoin.
	const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	requiredExtensions = { glfwExtensions, glfwExtensions + extensionCount };
	if (g_enableValidationLayers)
		requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName);

	// On récupère les extensions dont dispose notre contexte Vulkan
	std::vector<vk::ExtensionProperties> extensionsProperties = _context.enumerateInstanceExtensionProperties();
	// Pour chaque extension requises par GLFW, on regarde si elle existe dans notre contexte Vulkan.
	// Si une manque, alors on ne peut pas initialiser GLFW.
	for (const char * tmp : requiredExtensions)
	{
		bool foundExtension = false;
		for (vk::ExtensionProperties extension : extensionsProperties)
		{
			if (strcmp(extension.extensionName, tmp) == 0)
			{
				foundExtension = true;
				break;
			}
		}
		if (!foundExtension)
			throw std::runtime_error("Required GLFW extension not supported: " + std::string(tmp));
	}
	return requiredExtensions;
}

void VulkanEngine::_selectPhysicalDevice()
{
	std::vector<vk::raii::PhysicalDevice> devices = _instance.enumeratePhysicalDevices();

	if (devices.empty())
		throw std::runtime_error("Failed to find a GPU compatible with Vulkan.");

	std::map<uint32_t, vk::raii::PhysicalDevice> candidates;
	for (const vk::raii::PhysicalDevice & device : devices)
	{
		vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
		vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();
		uint32_t score = 0;

		if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			score += 1000;

		score += deviceProperties.limits.maxImageDimension2D;

		if (!deviceFeatures.geometryShader)
		   continue;
		candidates.insert(std::make_pair(score, device));
	}

	if (candidates.empty())
		throw std::runtime_error("Failed to find a suitable GPU");

	_physicalDevice = candidates.rbegin()->second;
	std::cout << GREEN << "[OK] Selected GPU: " << _physicalDevice.getProperties().deviceName << RESET << std::endl;
}

QueueIndices VulkanEngine::_findQueueFamilies() const
{
	QueueIndices queueIndices;
	// Trouve l'index de la première queue family
	const std::vector<vk::QueueFamilyProperties> queueFamilyProperties = _physicalDevice.getQueueFamilyProperties();
	uint32_t i = 0;

	// Trouve l'index de la queue family properties qui possède les graphics
	while (i < queueFamilyProperties.size())
	{
		if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			queueIndices.graphicsIndex = i;
			if (_physicalDevice.getSurfaceSupportKHR(i, _surface)) // Est-ce qu'on peut présenter des images à notre surface
				queueIndices.presentIndex = i;
			else
				continue;
			return queueIndices;
		}
		++i;
	}
	throw std::runtime_error("No queue family supporting graphics and present found.");
}

void VulkanEngine::_checkDeviceExtensions() const
{
	// On récupère les device extensions dont dispose notre GPU.
	std::vector<vk::ExtensionProperties> deviceExtensionProperties = _physicalDevice.enumerateDeviceExtensionProperties();
	// Pour chaque extension requises par Vulkan, on regarde si elle existe dans notre GPU.
	// Si une manque, alors on ne peut pas initialiser Vulkan.
	for (const char * tmp : g_deviceExtensions)
	{
		bool foundExtension = false;
		for (vk::ExtensionProperties extension : deviceExtensionProperties)
		{
			if (strcmp(extension.extensionName, tmp) == 0)
			{
				foundExtension = true;
				break;
			}
		}
		if (!foundExtension)
			throw std::runtime_error("Missing required device extension: " + std::string(tmp));
	}
}

void VulkanEngine::_createLogicalDevice()
{
	std::vector<vk::QueueFamilyProperties> qfp = _physicalDevice.getQueueFamilyProperties();
	float queuePriority = 0.0f;
	_queueIndices = _findQueueFamilies();

	vk::PhysicalDeviceFeatures supportedFeatures = _physicalDevice.getFeatures();
	if (!supportedFeatures.samplerAnisotropy)
		throw std::runtime_error("Missing anisotropy feature.");

	// Récupérer l'index de la queue family que l'on va utiliser
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
	deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
	deviceQueueCreateInfo.queueFamilyIndex = _queueIndices.graphicsIndex;
	deviceQueueCreateInfo.queueCount = 1; // Ligne qu'on a du rajouter pour indiquer que nous possédons 1 queue

	// Activation des features en liste chaînée
	vk::PhysicalDeviceFeatures2 features;
	features.features.sampleRateShading = vk::True; // Activer le MSAA (rajout hors-tutoriel)
	features.features.samplerAnisotropy = vk::True;
	vk::PhysicalDeviceVulkan13Features vulkan13features;
	vulkan13features.dynamicRendering = true;
	vulkan13features.synchronization2 = true; // Eviter un segfault (rajout hors-tutoriel)
	vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT dynamicStateFeatures;
	dynamicStateFeatures.extendedDynamicState = true;
	features.pNext = vulkan13features;
	vulkan13features.pNext = dynamicStateFeatures;
	dynamicStateFeatures.pNext = nullptr;

	// On remplit les infos de notre logical device
	vk::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.pNext = &features;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(g_deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = g_deviceExtensions.data();

	_device = vk::raii::Device(_physicalDevice, deviceCreateInfo);

	_queue = vk::raii::Queue(_device, _queueIndices.graphicsIndex, 0);

	if (g_enableValidationLayers)
		std::cout << GREEN << "[OK] Created Logical Device" << RESET << std::endl;
}

void VulkanEngine::_createSurface()
{
	VkSurfaceKHR surface;

	if (glfwCreateWindowSurface(*_instance, _window, nullptr, &surface) != 0)
		throw std::runtime_error("Failed to create window surface.");

	_surface = vk::raii::SurfaceKHR(_instance, surface);

	if (g_enableValidationLayers)
		std::cout << GREEN << "[OK] Created Surface" << RESET << std::endl;
}
