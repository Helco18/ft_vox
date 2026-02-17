#include "VulkanEngine.hpp"
#include "Logger.hpp"
#include "GLFW/glfw3.h"
#ifdef __has_include
	#if __has_include(<valgrind/valgrind.h>)
		#include <valgrind/valgrind.h>
		#define VALGRIND_AVAILABLE
	#endif
#endif

vk::SurfaceFormatKHR VulkanEngine::_chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> & formats)
{
	for (const vk::SurfaceFormatKHR & format : formats)
	{
		if (format.format == vk::Format::eR8G8B8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return format;
		// TODO Sélectionner le meilleur format de couleur disponible si celui que l'on veut n'est pas disponible.
	}
	return formats[0];
}

vk::PresentModeKHR VulkanEngine::_chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> & presentModes)
{
	#ifdef VALGRIND_AVAILABLE
	if (RUNNING_ON_VALGRIND)
		return vk::PresentModeKHR::eFifo;
	#endif
	if (!_vsync)
	{
		for (const vk::PresentModeKHR & presentMode : presentModes)
		{
			if (presentMode == vk::PresentModeKHR::eImmediate)
				return presentMode;
		}
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

	if (!_isInitalized)
		Logger::log(ENGINE_VULKAN, INFO, "Created Swapchain.");
}

void VulkanEngine::_createImageViews()
{
	_swapChainImageViews.clear();

	vk::ImageViewCreateInfo imageViewCreateInfo;
	// On utilise le 2D pour les opérations basiques.
	// On peut utiliser 1D pour des lookup tables par exemple.
	// On peut utiliser 3D pour des textures 3D (nuages, fumée, IRM...)
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.format = _swapChainSurfaceFormat.format;
	// Nos images vont être utilisées en tant que cible pour les couleurs, sans mipmap et avec un seul layer
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
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

	if (!_isInitalized)
		Logger::log(ENGINE_VULKAN, INFO, "Created Image Views.");
}

void VulkanEngine::_recreateSwapchain()
{
	int width = 0;
	int height = 0;

	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(_window, &width, &height);
		glfwWaitEvents();
	}

	_device.waitIdle();
	// _queue.waitIdle(); //Gael jte jure cette fois ca marchera

	_swapChainImageViews.clear();
	_swapChain = nullptr;

	_createSwapChain();
	_createImageViews();
	_createMultisamplingImage();
	_createDepthResources();
}
