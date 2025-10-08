#include "VulkanEngine.hpp"
#include <limits>

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
	std::cout << BOLD_MAGENTA << "Selected GPU: " << devices[0].getProperties().deviceName << RESET << std::endl;
	_physicalDevice = devices[0];
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

vk::SurfaceFormatKHR VulkanEngine::_chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> & formats)
{
	for (const vk::SurfaceFormatKHR & format : formats)
	{
		if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return format;
		// TODO Sélectionner le meilleur format de couleur disponible si celui que l'on veut n'est pas disponible.
	}
	return formats[0];
}

vk::PresentModeKHR VulkanEngine::_chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> & presentModes)
{
	for (const vk::PresentModeKHR & presentMode : presentModes)
	{
		if (presentMode == vk::PresentModeKHR::eImmediate)
			return presentMode;
	}
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanEngine::_chooseSwapExtent(const vk::SurfaceCapabilitiesKHR & capabilities)
{
	// On récupère la taille de l'écran dans currentExtent qui retourne le width et height.
	// Certains window managers mettent le currentExtent au max uint32.
	// Dans ce cas, on récupère la taille de l'écran autrement.
	if (capabilities.currentExtent != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;

	// On récupère le frame buffer size et on le clamp entre le minimum et maximum image extend
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);

	uint32_t effectiveWidth, effectiveHeight;
	effectiveWidth = std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	effectiveHeight = std::clamp<uint32_t>(height, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	return { effectiveWidth, effectiveHeight };
}

void VulkanEngine::_createLogicalDevice()
{
	std::vector<vk::QueueFamilyProperties> qfp = _physicalDevice.getQueueFamilyProperties();
	float queuePriority = 0.0f;
	_queueIndices = _findQueueFamilies();

	// Récupérer l'index de la queue family que l'on va utiliser
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
	deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
	deviceQueueCreateInfo.queueFamilyIndex = _queueIndices.graphicsIndex;
	deviceQueueCreateInfo.queueCount = 1; // Ligne qu'on a du rajouter pour indiquer que nous possédons 1 queue

	// Activation des features en liste chaînée
	vk::PhysicalDeviceFeatures2 features;
	vk::PhysicalDeviceVulkan13Features vulkan13features;
	vulkan13features.dynamicRendering = true;
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

	_graphicsQueue = vk::raii::Queue(_device, _queueIndices.graphicsIndex, 0);
}

void VulkanEngine::_createSurface()
{
	VkSurfaceKHR surface;

	if (glfwCreateWindowSurface(*_instance, _window, nullptr, &surface) != 0)
		throw std::runtime_error("Failed to create window surface.");

	_surface = vk::raii::SurfaceKHR(_instance, surface);
}

void VulkanEngine::_createSwapChain()
{
	// On récupère les capacités de notre surface
	// (min/max nombre d'images dans la swap chain & min/max width et height de nos images)
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = _physicalDevice.getSurfaceCapabilitiesKHR(_surface);
	// On récupère les formats supportés par notre surface
	// (pixel format : comment lire/interpréter les bits de chaque pixel (Canal, type de variable, taille, normalisé ou non))
	// (color space : comment interpréter les valeurs de couleur décodée (Courbe de transfert, gamut (étendue des couleurs possibles), type d'écram (sRGB, ...)))
	_swapChainSurfaceFormat = _chooseSwapSurfaceFormat(_physicalDevice.getSurfaceFormatsKHR(_surface));
	_swapChainExtent = _chooseSwapExtent(surfaceCapabilities);
	uint minImageCount = std::max(3u, surfaceCapabilities.minImageCount) + 1; // Avoir 1 image en réserve en plus

	// Si notre minimum d'image est supérieur aux maximum d'images de notre surface, alors on le cap à ce max
	if (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount)
		minImageCount = surfaceCapabilities.maxImageCount;

	vk::SwapchainCreateInfoKHR swapChainCreateInfo;
	swapChainCreateInfo.flags = vk::SwapchainCreateFlagsKHR();
	swapChainCreateInfo.surface = _surface;
	swapChainCreateInfo.minImageCount = minImageCount;
	swapChainCreateInfo.imageFormat = _swapChainSurfaceFormat.format;
	swapChainCreateInfo.imageColorSpace = _swapChainSurfaceFormat.colorSpace;
	swapChainCreateInfo.imageExtent = _swapChainExtent;
	// On spécifie le nombre de layers. C'est toujours 1 sauf si on fait une application stereoscopic 3D.
	swapChainCreateInfo.imageArrayLayers = 1;
	// On spécifie quel genre d'opérations nos images vont faire. On veut juste render dessus donc on met ColorAttachment.
	swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	// Si on n'autorise pas la rotation 90 degrés ou le flip horizontal on met currentTransform. Sinon, on met supportedTransforms.
	swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
	// On spécifie si on veut faire du blending avec les fenêtres derrière notre application ou non (opaque)
	swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	// On récupère les différents modes de présentation.
	// (Comment les images rendues dans la swap chain sont présentées à l'écran)
	// (IMMEDIATE : Affichée dès qu'elle est prête sans attendre vsync)
	// (FIFO : L'image est présentée à chaque 'vblank' (le signal de vsync))
	// (FIFO_RELAXED : Comme FIFO, sauf que si le GPU rate le vblank, l'image est affichée dès qu'elle est prête)
	// (MAILBOX : Au lieu de bloquer l'application quand la queue est pleine, les images qui sont dans la queue sont remplacée par les nouvelles (triple buffering))
	swapChainCreateInfo.presentMode = _chooseSwapPresentMode(_physicalDevice.getSurfacePresentModesKHR(_surface));
	// Si true, on ne lit pas les pixels obstrués par une autre fenêtre devant notre application ou si la fenêtre sort de l'écran
	swapChainCreateInfo.clipped = true;
	// On précise un pointeur vers l'ancienne swapchain car elle est détruite lorsque la fenêtre est resize.
	swapChainCreateInfo.oldSwapchain = nullptr;

	// Si nous avons 2 queue différentes pour graphics et present, on les mets en concurrent (Les images peuvent être utilisées par chaque queue family)
	// Sinon, on met la queue en exclusive (Les images ne peuvent être utilisées que par 1 seule queue family à la fois)
	uint32_t indices[] = { _queueIndices.graphicsIndex, _queueIndices.presentIndex };
	if (_queueIndices.graphicsIndex != _queueIndices.presentIndex)
	{
		swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices = indices;
	}
	else
	{
		swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
		swapChainCreateInfo.queueFamilyIndexCount = 0;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	_swapChainImageFormat = _swapChainSurfaceFormat.format;
	_swapChain = vk::raii::SwapchainKHR(_device, swapChainCreateInfo);
	_swapChainImages = _swapChain.getImages();
}

void VulkanEngine::_createImageViews()
{
	_swapChainImages.clear();

	vk::ImageViewCreateInfo imageViewCreateInfo;
	// On utilise le 2D pour les opérations basiques.
	// On peut utiliser 1D pour des lookup tables par exemple.
	// On peut utiliser 3D pour des textures 3D (nuages, fumée, IRM...)
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.format = _swapChainImageFormat;
	imageViewCreateInfo.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
	// Nos images vont être utilisées en tant que cible pour les couleurs, sans mipmap et avec un seul layer
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlags::BitsType::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 1;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	// Mappings de couleur (ici, par défaut)
	imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
	// On ajoute chaque image à notre structure
	for (vk::Image image : _swapChainImages)
	{
		imageViewCreateInfo.image = image;
		_swapChainImageViews.emplace_back(_device, imageViewCreateInfo);
	}
}
