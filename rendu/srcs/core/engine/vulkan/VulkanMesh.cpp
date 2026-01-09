#include "CustomExceptions.hpp"
#include "VulkanEngine.hpp"
#include "utils.hpp"

vk::VertexInputBindingDescription VulkanEngine::_getBindingDescription(PipelineInfo & pipelineInfo) const
{
	// Layout id, stride (taille de ta donnée), vertex ou instance
	return { 0, pipelineInfo.attributeSize, vk::VertexInputRate::eVertex };
}

VulkanEngine::VertexAttributeDescriptionVector VulkanEngine::_getAttributeDescription(PipelineInfo & pipelineInfo) const
{
	VertexAttributeDescriptionVector vadVector;
	std::vector<Attribute> attributes = pipelineInfo.attributes;
	size_t offset = 0;

	// Spécifier ce que notre vertex a comme attributs. Ici, nous avons sa position dans la location 0 et sa couleur dans la location 1.
	// On stocke des coordonnées en couleur.
	for (size_t i = 0; i < attributes.size(); ++i)
	{
		vadVector.push_back(vk::VertexInputAttributeDescription(i, 0, VKValueConverter::getType(attributes[i].type),
			offset));
		offset += attributes[i].size;
	}
	return vadVector;
}

uint32_t VulkanEngine::_findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties = _physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}
	throw VulkanException("Couldn't find suitable memory type.");
}

void VulkanEngine::_createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
									vk::raii::Buffer & buffer, vk::raii::DeviceMemory & deviceMemory)
{
	vk::BufferCreateInfo bufferInfo;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	buffer = vk::raii::Buffer(_device, bufferInfo);

	vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();

	vk::MemoryAllocateInfo memAllocateInfo;
	memAllocateInfo.allocationSize = memRequirements.size;
	memAllocateInfo.memoryTypeIndex = _findMemoryType(memRequirements.memoryTypeBits, properties);
	
	deviceMemory = vk::raii::DeviceMemory(_device, memAllocateInfo);
	buffer.bindMemory(*deviceMemory, 0);
}

vk::raii::CommandBuffer VulkanEngine::_beginSingleTimeCommands()
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.commandPool = _transientCommandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	vk::raii::CommandBuffer commandCopyBuffer = std::move(_device.allocateCommandBuffers(allocInfo).front());

	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandCopyBuffer.begin(commandBufferBeginInfo);

	return commandCopyBuffer;
}

void VulkanEngine::_endSingleTimeCommands(vk::raii::CommandBuffer & commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &*commandBuffer;
	_queue.submit(submitInfo);
	_queue.waitIdle();
}

void VulkanEngine::_copyBuffer(vk::raii::Buffer & srcBuffer, vk::raii::Buffer & dstBuffer, vk::DeviceSize size)
{
	vk::raii::CommandBuffer commandCopyBuffer = _beginSingleTimeCommands();
	commandCopyBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
	_endSingleTimeCommands(commandCopyBuffer);
}

void VulkanEngine::_concateneVertexBuffer(Asset & asset)
{
	if (asset.vertices.empty())
		return;
	vk::DeviceSize size = sizeof(Vertex) * (asset.vertices.size() + _vertexSize);
	vk::raii::Buffer stagingBuffer = nullptr;
	vk::raii::DeviceMemory stagingBufferMemory = nullptr;

	_createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc,
					vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
					stagingBuffer, stagingBufferMemory);

	void * dataStaging = stagingBufferMemory.mapMemory(0, size);
	_vertices.insert(_vertices.end(), asset.vertices.begin(), asset.vertices.end());
	memcpy(dataStaging, _vertices.data(), size);
	stagingBufferMemory.unmapMemory();

	_createBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, 
					vk::MemoryPropertyFlagBits::eDeviceLocal,
					_vertexBuffer, _vertexBufferMemory);
	_copyBuffer(stagingBuffer, _vertexBuffer, size);

	_vertexSize = _vertices.size();
}

void VulkanEngine::_concateneIndexBuffer(Asset & asset)
{
	if (asset.indices.empty())
		return;
	vk::DeviceSize size = sizeof(asset.indices[0]) * (asset.indices.size() + _indexSize);
	vk::raii::Buffer stagingBuffer = nullptr;
	vk::raii::DeviceMemory stagingBufferMemory = nullptr;

	_createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, 
					vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
					stagingBuffer, stagingBufferMemory);

	void * dataStaging = stagingBufferMemory.mapMemory(0, size);
	_indices.insert(_indices.end(), asset.indices.begin(), asset.indices.end());
	memcpy(dataStaging, _indices.data(), size);
	stagingBufferMemory.unmapMemory();

	_createBuffer(size, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, 
					vk::MemoryPropertyFlagBits::eDeviceLocal,
					_indexBuffer, _indexBufferMemory);
	_copyBuffer(stagingBuffer, _indexBuffer, size);

	_indexSize = _indices.size();
}
