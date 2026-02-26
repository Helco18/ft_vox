#include "CustomExceptions.hpp"
#include "VulkanEngine.hpp"

vk::VertexInputBindingDescription VulkanEngine::_getBindingDescription(PipelineInfo & pipelineInfo) const
{
	// Layout id, stride (taille de ta donnée), vertex ou instance
	return { 0, pipelineInfo.attributeSize, vk::VertexInputRate::eVertex };
}

VulkanEngine::VertexAttributeDescriptionVector VulkanEngine::_getAttributeDescription(PipelineInfo & pipelineInfo) const
{
	VertexAttributeDescriptionVector vadVector;
	const std::vector<Attribute> & attributes = pipelineInfo.attributes;
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

void VulkanEngine::_createVertexBuffer(PendingAsset & pendingAsset)
{
	if (!_isInitalized.load())
		return;

	Asset * asset = pendingAsset.asset;
	BufferData & vertexData = pendingAsset.vertexData;
	BufferData & stagingVertexData = pendingAsset.stagingVertexData;
	vk::DeviceSize size = asset->vertices.size;

	_createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc,
					vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
					stagingVertexData.buffer, stagingVertexData.memory);

	void * dataStaging = stagingVertexData.memory.mapMemory(0, size);
	memcpy(dataStaging, asset->vertices.data, size);
	stagingVertexData.memory.unmapMemory();

	_createBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, 
					vk::MemoryPropertyFlagBits::eDeviceLocal,
					vertexData.buffer, vertexData.memory);
}

void VulkanEngine::_createIndexBuffer(PendingAsset & pendingAsset)
{
	if (!_isInitalized.load())
		return;

	Asset * asset = pendingAsset.asset;
	BufferData & indexData = pendingAsset.indexData;
	BufferData & stagingIndexData = pendingAsset.stagingIndexData;
	vk::DeviceSize size = sizeof(asset->indices[0]) * asset->indices.size();

	_createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, 
					vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
					stagingIndexData.buffer, stagingIndexData.memory);

	void * dataStaging = stagingIndexData.memory.mapMemory(0, size);
	memcpy(dataStaging, asset->indices.data(), size);
	stagingIndexData.memory.unmapMemory();

	_createBuffer(size, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, 
					vk::MemoryPropertyFlagBits::eDeviceLocal,
					indexData.buffer, indexData.memory);
}
