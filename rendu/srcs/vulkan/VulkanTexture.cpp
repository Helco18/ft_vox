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
	image.bindMemory(imageMemory, 0);
}

void VulkanEngine::_createTextureImage()
{
	int width, height;
	OBJModel model;
	Texture texture;

	model = OBJModel::getModel(TEST);
	texture = model.getTexture();
	width = texture.width;
	height = texture.height;
	stbi_uc * pixels = texture.data;
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

	_createImage(width, height, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
					vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
					vk::MemoryPropertyFlagBits::eDeviceLocal, _textureImage, _textureImageMemory);
	
	_transitionImageLayout(_textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    _copyBufferToImage(stagingBuffer, _textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    _transitionImageLayout(_textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}

void VulkanEngine::_copyBufferToImage(const vk::raii::Buffer & buffer, vk::raii::Image & image, uint32_t width, uint32_t height)
{
	vk::raii::CommandBuffer commandBuffer = _beginSingleTimeCommands();

	vk::BufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = vk::Offset3D(0, 0, 0);
	region.imageExtent = vk::Extent3D(width, height, 1);
	
	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, { region });
	_endSingleTimeCommands(commandBuffer);
}

void VulkanEngine::_createTextureImageView()
{
	vk::ImageViewCreateInfo imageViewCreateInfo;
	// On utilise le 2D pour les opérations basiques.
	// On peut utiliser 1D pour des lookup tables par exemple.
	// On peut utiliser 3D pour des textures 3D (nuages, fumée, IRM...)
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.image = _textureImage;
	imageViewCreateInfo.format = vk::Format::eR8G8B8A8Srgb;
	imageViewCreateInfo.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
	// Nos images vont être utilisées en tant que cible pour les couleurs, sans mipmap et avec un seul layer
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlags::BitsType::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	_textureImageView = vk::raii::ImageView( _device, imageViewCreateInfo );
}

void VulkanEngine::_createTextureSampler()
{
	vk::PhysicalDeviceProperties properties = _physicalDevice.getProperties();

	vk::SamplerCreateInfo samplerInfo;
	samplerInfo.flags = {};
	samplerInfo.magFilter = vk::Filter::eLinear;
	samplerInfo.minFilter = vk::Filter::eLinear;
	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
	samplerInfo.mipLodBias = 0;
	samplerInfo.anisotropyEnable = 1;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.compareEnable = vk::False;
	samplerInfo.compareOp = vk::CompareOp::eAlways;
	samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = vk::False;

	_textureSampler = vk::raii::Sampler( _device, samplerInfo );
}
