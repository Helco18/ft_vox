#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"

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
	throw VulkanException("Failed to find supported Depth format.");
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

void VulkanEngine::_transitionDepthImage()
{
	vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    vk::raii::CommandBuffer cmd = std::move(_device.allocateCommandBuffers(allocInfo).front());

	// Depth testing
	vk::ImageMemoryBarrier2 depthBarrier;
	depthBarrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
	depthBarrier.srcAccessMask = {};
	depthBarrier.dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests;
	depthBarrier.dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
	depthBarrier.oldLayout = vk::ImageLayout::eUndefined;
	depthBarrier.newLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	depthBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	depthBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	depthBarrier.image = _depthImage;
	depthBarrier.subresourceRange.aspectMask = _depthFlags;
	depthBarrier.subresourceRange.baseMipLevel = 0;
	depthBarrier.subresourceRange.levelCount = 1;
	depthBarrier.subresourceRange.baseArrayLayer = 0;
	depthBarrier.subresourceRange.layerCount = 1;

	vk::DependencyInfo depthDependencyInfo;
	depthDependencyInfo.imageMemoryBarrierCount = 1;
	depthDependencyInfo.pImageMemoryBarriers = &depthBarrier;

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	cmd.begin(beginInfo);
	cmd.pipelineBarrier2(depthDependencyInfo);
	cmd.end();

	vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &*cmd;
	
	_queue.submit(submitInfo);
	_queue.waitIdle();
}

void VulkanEngine::_createDepthResources()
{
	_depthFormat = _findDepthFormat();
	_depthFlags = vk::ImageAspectFlagBits::eDepth;
	if (_depthFormat != vk::Format::eD32Sfloat)
		_depthFlags |= vk::ImageAspectFlagBits::eStencil;

	_createImage(_swapChainExtent.width, _swapChainExtent.height, _depthFormat, vk::ImageTiling::eOptimal, 
				vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, 
				_depthImage, _depthImageMemory, vk::SampleCountFlagBits::e1);

	vk::ImageViewCreateInfo viewInfo;
	viewInfo.image = _depthImage;
	viewInfo.format = _depthFormat;
	viewInfo.viewType = vk::ImageViewType::e2D;
	viewInfo.subresourceRange.aspectMask = _depthFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	_depthImageView = vk::raii::ImageView( _device, viewInfo );
	_transitionDepthImage();
}
