#include "VulkanEngine.hpp"
#include "Logger.hpp"
#include <iostream>

void VulkanEngine::_createDescriptorSetLayout()
{
	// Décrit la structure des ressources accessibles par le shader

	vk::DescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	vk::DescriptorSetLayoutBinding texLayoutBinding;
	texLayoutBinding.binding = 1;
	texLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	texLayoutBinding.descriptorCount = 1;
	texLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
	texLayoutBinding.pImmutableSamplers = nullptr;

	std::array descriptorLayouts = { uboLayoutBinding, texLayoutBinding };

	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.flags = {};
	layoutInfo.bindingCount = descriptorLayouts.size();
	layoutInfo.pBindings = descriptorLayouts.data();

	_descriptorSetLayout = vk::raii::DescriptorSetLayout( _device, layoutInfo );

	if (g_enableValidationLayers)
		Logger::log(ENGINE_VULKAN, INFO, "Created Descriptor Pool Set.");
}

void VulkanEngine::_createDescriptorPool()
{
	vk::DescriptorPoolSize uniformSize;
	uniformSize.type = vk::DescriptorType::eUniformBuffer;
	uniformSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

	vk::DescriptorPoolSize textureSize;
	textureSize.type = vk::DescriptorType::eCombinedImageSampler;
	textureSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

	std::array descriptorsSize = { uniformSize, textureSize };

	vk::DescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	descriptorPoolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
	descriptorPoolInfo.poolSizeCount = descriptorsSize.size();
	descriptorPoolInfo.pPoolSizes = descriptorsSize.data();

	_descriptorPool = vk::raii::DescriptorPool( _device, descriptorPoolInfo );

	if (g_enableValidationLayers)
		Logger::log(ENGINE_VULKAN, INFO, "Created Descriptor Pool.");
}

void VulkanEngine::_createDescriptorSets()
{
	std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *_descriptorSetLayout);
	vk::DescriptorSetAllocateInfo descriptorSetAllocInfo;
	descriptorSetAllocInfo.descriptorPool = _descriptorPool;
	descriptorSetAllocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
	descriptorSetAllocInfo.pSetLayouts = layouts.data();

	_descriptorSets.clear();
	_descriptorSets = _device.allocateDescriptorSets(descriptorSetAllocInfo);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vk::DescriptorBufferInfo uniformBufferInfo;
		uniformBufferInfo.buffer = _uniformBuffers[i];
		uniformBufferInfo.offset = 0;
		uniformBufferInfo.range = sizeof(UniformBuffer);

		vk::DescriptorImageInfo textureBufferInfo;
		textureBufferInfo.sampler = _textureSampler;
		textureBufferInfo.imageView = _textureImageView;
		textureBufferInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

		vk::WriteDescriptorSet uniformWrite;
		uniformWrite.dstSet = _descriptorSets[i];
		uniformWrite.dstBinding = 0;
		uniformWrite.dstArrayElement = 0;
		uniformWrite.descriptorCount = 1;
		uniformWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
		uniformWrite.pBufferInfo = &uniformBufferInfo;

		vk::WriteDescriptorSet textureWrite;
		textureWrite.dstSet = _descriptorSets[i];
		textureWrite.dstBinding = 1;
		textureWrite.dstArrayElement = 0;
		textureWrite.descriptorCount = 1;
		textureWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		textureWrite.pImageInfo = &textureBufferInfo;

		std::array writeSets = { uniformWrite, textureWrite };

		_device.updateDescriptorSets(writeSets, {});
	}

	if (g_enableValidationLayers)
		Logger::log(ENGINE_VULKAN, INFO, "Created Descriptor Sets.");
}
