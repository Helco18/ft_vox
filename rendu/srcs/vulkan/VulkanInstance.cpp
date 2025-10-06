#include "VulkanEngine.hpp"

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
	for (uint32_t i = 0; i < requiredExtensions.size(); ++i)
	{
		bool foundExtension = false;
		const char * tmp = requiredExtensions[i];
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
}
