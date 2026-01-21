#include "CustomExceptions.hpp"
#include "VulkanEngine.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <iostream>

void VulkanEngine::_createDescriptorPool(PipelineData & pipelineData)
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

	pipelineData.descriptorPool = vk::raii::DescriptorPool( _device, descriptorPoolInfo );

	Logger::log(ENGINE_VULKAN, INFO, "Created Descriptor Pool.");
}

void VulkanEngine::_createDescriptorSetLayout(PipelineData & pipelineData)
{
	std::vector<vk::DescriptorSetLayoutBinding> descriptorLayouts;

	for (DescriptorInfo & descriptorInfo : pipelineData.pipelineInfo.descriptors)
	{
		// Décrit la structure des ressources accessibles par le shader
		vk::DescriptorSetLayoutBinding layoutBinding;
		layoutBinding.binding = descriptorInfo.binding;
		layoutBinding.descriptorType = VKValueConverter::getDescriptorType(descriptorInfo.type);
		layoutBinding.descriptorCount = descriptorInfo.count;
		layoutBinding.stageFlags = VKValueConverter::getShaderStage(descriptorInfo.stage);
		layoutBinding.pImmutableSamplers = nullptr;

		descriptorLayouts.push_back(layoutBinding);
	}

	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.flags = {};
	layoutInfo.bindingCount = descriptorLayouts.size();
	layoutInfo.pBindings = descriptorLayouts.data();

	pipelineData.descriptorSetLayout = vk::raii::DescriptorSetLayout( _device, layoutInfo );
	Logger::log(ENGINE_VULKAN, INFO, "Created Descriptor Set Layout.");
}

void VulkanEngine::_createDescriptorSets(PipelineData & pipelineData)
{
	std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *pipelineData.descriptorSetLayout);
	vk::DescriptorSetAllocateInfo descriptorSetAllocInfo;
	descriptorSetAllocInfo.descriptorPool = pipelineData.descriptorPool;
	descriptorSetAllocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
	descriptorSetAllocInfo.pSetLayouts = layouts.data();

	pipelineData.descriptorSets = _device.allocateDescriptorSets(descriptorSetAllocInfo);

	for (const DescriptorInfo & descriptorInfo : pipelineData.pipelineInfo.descriptors)
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vk::WriteDescriptorSet writeSet;
			writeSet.dstSet = pipelineData.descriptorSets[i];
			writeSet.dstBinding = descriptorInfo.binding;
			writeSet.dstArrayElement = 0;
			writeSet.descriptorCount = descriptorInfo.count;
			writeSet.descriptorType = VKValueConverter::getDescriptorType(descriptorInfo.type);

			if (descriptorInfo.type == UNIFORM_BUFFER)
			{
				vk::DescriptorBufferInfo uniformBufferInfo;
				uniformBufferInfo.buffer = pipelineData.uniforms.bufferData[i].buffer;
				uniformBufferInfo.offset = 0;
				uniformBufferInfo.range = pipelineData.uniforms.size;
				writeSet.pBufferInfo = &uniformBufferInfo;
				_device.updateDescriptorSets(writeSet, {});
			}
			else if (descriptorInfo.type == COMBINED_IMAGE_SAMPLER)
			{
				vk::DescriptorImageInfo textureBufferInfo;
				textureBufferInfo.sampler = pipelineData.textures.textureSampler;
				textureBufferInfo.imageView = pipelineData.textures.textureImageView;
				textureBufferInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
				writeSet.pImageInfo = &textureBufferInfo;
				_device.updateDescriptorSets(writeSet, {});
			}
			else
				throw VulkanException("Unknown descriptor found for pipeline ID: " + toString(pipelineData.pipelineInfo.id));
		}
	}
	Logger::log(ENGINE_VULKAN, INFO, "Created Descriptor Sets.");
}
