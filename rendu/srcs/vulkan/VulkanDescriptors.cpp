#include "VulkanEngine.hpp"

void VulkanEngine::_createDescriptorSetLayout()
{
	// Décrit la structure des ressources accessibles par le shader
	vk::DescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.bindingCount = 1;
	layoutInfo.flags = {};
	layoutInfo.pBindings = &uboLayoutBinding;

	_descriptorSetLayout = vk::raii::DescriptorSetLayout( _device, layoutInfo );

	if (g_enableValidationLayers)
		std::cout << GREEN << "[OK] Created Descriptor Pool Set" << RESET << std::endl;
}

void VulkanEngine::_createDescriptorPool()
{
	vk::DescriptorPoolSize size;
	size.type = vk::DescriptorType::eUniformBuffer;
	size.descriptorCount = MAX_FRAMES_IN_FLIGHT;

	vk::DescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	descriptorPoolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
	descriptorPoolInfo.poolSizeCount = 1;
	descriptorPoolInfo.pPoolSizes = &size;

	_descriptorPool = vk::raii::DescriptorPool( _device, descriptorPoolInfo );

	if (g_enableValidationLayers)
		std::cout << GREEN << "[OK] Created Descriptor Pool" << RESET << std::endl;
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
		vk::DescriptorBufferInfo bufferInfo;
		bufferInfo.buffer = _uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBuffer);

		vk::WriteDescriptorSet descriptorWrite;
		descriptorWrite.dstSet = _descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrite.pBufferInfo = &bufferInfo;

		_device.updateDescriptorSets(descriptorWrite, {});
	}

	if (g_enableValidationLayers)
		std::cout << GREEN << "[OK] Created Descriptor Sets" << RESET << std::endl;
}
