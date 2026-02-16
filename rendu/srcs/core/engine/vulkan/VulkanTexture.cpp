#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"

void VulkanEngine::_createTextureImage(TextureData & textureData, TextureInfo & textureInfo)
{
	int width, height, colorChannels;

	width = textureInfo.width;
	height = textureInfo.height;
	colorChannels = textureInfo.colorChannels;
	void * pixels = textureInfo.data;
	vk::DeviceSize size = width * height * colorChannels;
	if (!pixels)
		throw VulkanException("Failed to load image.");

	vk::raii::Buffer stagingBuffer = nullptr;
	vk::raii::DeviceMemory stagingBufferMemory = nullptr;
	vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

	_createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, properties, stagingBuffer, stagingBufferMemory);
	void * data = stagingBufferMemory.mapMemory(0, size);
	memcpy(data, pixels, size);
	stagingBufferMemory.unmapMemory();

	_createImage(width, height, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal,
					vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
					vk::MemoryPropertyFlagBits::eDeviceLocal, textureData.image, textureData.memory,
					vk::SampleCountFlagBits::e1);
	
	_transitionImageLayout(textureData.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    _copyBufferToImage(stagingBuffer, textureData.image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    _transitionImageLayout(textureData.image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
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

void VulkanEngine::_createTextureImageView(TextureData & textureData)
{
	vk::ImageViewCreateInfo imageViewCreateInfo;
	// On utilise le 2D pour les opérations basiques.
	// On peut utiliser 1D pour des lookup tables par exemple.
	// On peut utiliser 3D pour des textures 3D (nuages, fumée, IRM...)
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.image = textureData.image;
	imageViewCreateInfo.format = vk::Format::eR8G8B8A8Unorm;
	// Nos images vont être utilisées en tant que cible pour les couleurs, sans mipmap et avec un seul layer
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlags::BitsType::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	textureData.textureImageView = vk::raii::ImageView( _device, imageViewCreateInfo );
}

void VulkanEngine::_createTextureSampler(TextureData & textureData, TextureInfo & textureInfo)
{
	vk::SamplerCreateInfo samplerInfo;
	samplerInfo.flags = {};
	samplerInfo.minFilter = VKValueConverter::getFilter(textureInfo.filtering);
	samplerInfo.magFilter = VKValueConverter::getFilter(textureInfo.filtering);
	samplerInfo.mipmapMode = VKValueConverter::getSamplerFilter(textureInfo.filtering);
	samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
	samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
	samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.compareEnable = vk::False;
	samplerInfo.compareOp = vk::CompareOp::eAlways;
	samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = vk::False;

	textureData.textureID = _nextTextureID++;
	textureData.textureSampler = vk::raii::Sampler( _device, samplerInfo );
}

void VulkanEngine::_createTextures(PipelineData & pipelineData)
{
	for (DescriptorInfo & descriptorInfo : pipelineData.pipelineInfo.descriptors)
	{
		if (descriptorInfo.type == DescriptorType::COMBINED_IMAGE_SAMPLER)
		{
			TextureData & textureData = pipelineData.textures[descriptorInfo.binding];
			_createTextureImage(textureData, descriptorInfo.textureInfo);
			_createTextureImageView(textureData);
			_createTextureSampler(textureData, descriptorInfo.textureInfo);
		}
	}
}
