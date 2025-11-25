#include "VulkanEngine.hpp"

vk::Format VulkanEngine::_findSupportedFormat(const std::vector<vk::Format> & candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	for (vk::Format candidate : candidates)
	{
		vk::FormatProperties props = _physicalDevice.getFormatProperties(candidate);
		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
			return candidate;
		if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
			return candidate;
	}
	throw std::runtime_error("Failed to find supported Depth format.");
}

vk::Format VulkanEngine::_findDepthFormat()
{
	std::vector<vk::Format> formats = { 
		vk::Format::eD24UnormS8Uint,
		vk::Format::eD32Sfloat,
		vk::Format::eD32SfloatS8Uint
	};
	return _findSupportedFormat(formats, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

void VulkanEngine::_createDepthResources()
{
	vk::Format depthFormat = _findDepthFormat();

	_createImage(_swapChainExtent.width, _swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment,
					vk::MemoryPropertyFlagBits::eDeviceLocal, _depthImage, _depthImageMemory, vk::SampleCountFlagBits::e1);

	vk::ImageViewCreateInfo viewInfo;
	viewInfo.image = _depthImage;
	viewInfo.format = depthFormat;
	viewInfo.viewType = vk::ImageViewType::e2D;
	viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	_depthImageView = vk::raii::ImageView( _device, viewInfo );
}
