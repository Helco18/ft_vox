#include "VulkanEngine.hpp"

void VulkanEngine::_createMultisamplingImage()
{
	_createImage(_swapChainExtent.width, _swapChainExtent.height,
		_swapChainImageFormat, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment,
		vk::MemoryPropertyFlagBits::eDeviceLocal, _msaaImage, _msaaImageMemory, vk::SampleCountFlagBits::e4);
	
	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = _msaaImage;
	// On utilise le 2D pour les opérations basiques.
	// On peut utiliser 1D pour des lookup tables par exemple.
	// On peut utiliser 3D pour des textures 3D (nuages, fumée, IRM...)
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.format = _swapChainImageFormat;
	// Nos images vont être utilisées en tant que cible pour les couleurs, sans mipmap et avec un seul layer
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	_msaaImageView = vk::raii::ImageView(_device, imageViewCreateInfo);
}
