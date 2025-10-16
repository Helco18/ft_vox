#include "VulkanEngine.hpp"

vk::VertexInputBindingDescription VulkanEngine::_getBindingDescription() const
{
	// Layout id, stride (taille de ta donnée), vertex ou instance
	return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
}

VulkanEngine::VertexAttributeDescriptionArray VulkanEngine::_getAttributeDescription() const
{
	// Spécifier ce que notre vertex a comme attributs. Ici, nous avons sa position dans la location 0 et sa couleur dans la location 1.
	// On stocke des coordonnées en couleur.
	return {
		vk::VertexInputAttributeDescription( 0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)),
		vk::VertexInputAttributeDescription( 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color))
	};
}

uint32_t VulkanEngine::_findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties = _physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}
	throw std::runtime_error("Couldn't find suitable memory type.");
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

void VulkanEngine::_copyBuffer(vk::raii::Buffer & srcBuffer, vk::raii::Buffer & dstBuffer, vk::DeviceSize size)
{
	// TODO Optimize Staging Buffer
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.commandPool = _transientCommandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	vk::raii::CommandBuffer commandCopyBuffer = std::move(_device.allocateCommandBuffers(allocInfo).front());

	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandCopyBuffer.begin(commandBufferBeginInfo);
	commandCopyBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
	commandCopyBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &*commandCopyBuffer;
	_queue.submit(submitInfo);
	_queue.waitIdle();
}

void VulkanEngine::_createVertexBuffer(ModelType type)
{
	const std::vector<Vertex> vertices = Model::getModel(type).getVertices();
	vk::DeviceSize size = sizeof(vertices[0]) * vertices.size();
	vk::raii::Buffer stagingBuffer = nullptr;
	vk::raii::DeviceMemory stagingBufferMemory = nullptr;

	_createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, 
					vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
					stagingBuffer, stagingBufferMemory);

	void * dataStaging = stagingBufferMemory.mapMemory(0, size);
	memcpy(dataStaging, vertices.data(), size);
	stagingBufferMemory.unmapMemory();

	_createBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, 
					vk::MemoryPropertyFlagBits::eDeviceLocal,
					_vertexBuffer, _vertexBufferMemory);
	_copyBuffer(stagingBuffer, _vertexBuffer, size);

	_vertexSize = vertices.size();
}

void VulkanEngine::_createIndexBuffer(ModelType type)
{
	const std::vector<uint16_t> indices = Model::getModel(type).getIndices();
	vk::DeviceSize size = sizeof(indices[0]) * indices.size();
	vk::raii::Buffer stagingBuffer = nullptr;
	vk::raii::DeviceMemory stagingBufferMemory = nullptr;

	_createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, 
					vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
					stagingBuffer, stagingBufferMemory);

	void * dataStaging = stagingBufferMemory.mapMemory(0, size);
	memcpy(dataStaging, indices.data(), size);
	stagingBufferMemory.unmapMemory();

	_createBuffer(size, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, 
					vk::MemoryPropertyFlagBits::eDeviceLocal,
					_indexBuffer, _indexBufferMemory);
	_copyBuffer(stagingBuffer, _indexBuffer, size);

	_indexSize = indices.size();
}
