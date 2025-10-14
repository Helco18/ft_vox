#include "VulkanEngine.hpp"

std::vector<Vertex> VulkanEngine::getVertexFromFile(const std::string & path)
{
	std::vector<Vertex> vertices;
	std::string tmp;
	std::ifstream file(path);
	std::vector<std::string> splitted;
	
	if (file.fail())
		throw std::runtime_error("No file found at given path: " + path);

	while (std::getline(file, tmp))
	{
		splitted = ft_split(tmp, ',');
		if (splitted.size() < 5)
			throw std::runtime_error("Invalid line at: " + tmp);
		glm::vec2 position(std::atof(splitted[0].c_str()), std::atof(splitted[1].c_str()));
		glm::vec3 color(std::atof(splitted[2].c_str()), std::atof(splitted[3].c_str()), std::atof(splitted[4].c_str()));
		vertices.push_back({ position, color });
	}
	return vertices;
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

void VulkanEngine::_createVertexBuffer(const std::string & modelpath)
{
	const std::vector<Vertex> vertices = getVertexFromFile(modelpath);

	vk::BufferCreateInfo bufferInfo;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	_vertexBuffer = vk::raii::Buffer(_device, bufferInfo);

	vk::MemoryRequirements memRequirements = _vertexBuffer.getMemoryRequirements();

	vk::MemoryAllocateInfo memAllocateInfo;
	memAllocateInfo.allocationSize = memRequirements.size;
	memAllocateInfo.memoryTypeIndex = _findMemoryType(memRequirements.memoryTypeBits,
											vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	
	_vertexBufferMemory = vk::raii::DeviceMemory(_device, memAllocateInfo);
	_vertexBuffer.bindMemory(*_vertexBufferMemory, 0);

	void * data = _vertexBufferMemory.mapMemory(0, bufferInfo.size);
	memcpy(data, vertices.data(), bufferInfo.size);
	_vertexBufferMemory.unmapMemory();

	_vertexSize = vertices.size();
}
