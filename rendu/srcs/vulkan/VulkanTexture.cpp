#include "VulkanEngine.hpp"
#include "stb/stb_image.h"

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
		throw std::runtime_error("Unsupported image layout transition");

	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
	_endSingleTimeCommands(commandBuffer);
}

void VulkanEngine::_createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
								vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image & image,
								vk::raii::DeviceMemory & imageMemory)
{
	vk::ImageCreateInfo imageInfo;
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.format = format;
	imageInfo.extent = vk::Extent3D(width, height ,1);
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.tiling = tiling;
	imageInfo.usage = usage;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;

	image = vk::raii::Image( _device, imageInfo );

	vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = _findMemoryType(memRequirements.memoryTypeBits, properties);
	imageMemory = vk::raii::DeviceMemory( _device, allocInfo );
}

void VulkanEngine::_createTextureImage()
{
	int width, height, channels;

	stbi_uc * pixels = stbi_load("assets/textures/gcannaud.jpg", &width, &height, &channels, STBI_rgb_alpha);
	vk::DeviceSize size = width * height * 4;
	if (!pixels)
		throw std::runtime_error("Failed to load image.");

	vk::raii::Buffer stagingBuffer = nullptr;
	vk::raii::DeviceMemory stagingBufferMemory = nullptr;
	vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

	_createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, properties, stagingBuffer, stagingBufferMemory);
	void * data = stagingBufferMemory.mapMemory(0, size);
	memcpy(data, pixels, size);
	stagingBufferMemory.unmapMemory();
	stbi_image_free(pixels);

	vk::raii::Image textureImage = nullptr;
	vk::raii::DeviceMemory textureImageMemory = nullptr;
	_createImage(width, height, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);
}
