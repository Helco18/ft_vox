#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"

void VulkanEngine::_transitionImageLayout(const vk::raii::Image & image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	vk::raii::CommandBuffer commandBuffer = _beginSingleTimeCommands();

	vk::ImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;

	vk::ImageMemoryBarrier barrier;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.image = image;
	barrier.subresourceRange = subresourceRange;

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else
		throw VulkanException("Unsupported image layout transition");

	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
	_endSingleTimeCommands(commandBuffer);
}

void VulkanEngine::_createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
								vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image & image,
								vk::raii::DeviceMemory & imageMemory, vk::SampleCountFlagBits sampling)
{
	vk::ImageCreateInfo imageInfo;
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.format = format;
	imageInfo.extent = vk::Extent3D(width, height ,1);
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = sampling;
	imageInfo.tiling = tiling;
	imageInfo.usage = usage;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;

	image = vk::raii::Image( _device, imageInfo );

	vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = _findMemoryType(memRequirements.memoryTypeBits, properties);
	imageMemory = vk::raii::DeviceMemory( _device, allocInfo );
	image.bindMemory(imageMemory, 0);
}
