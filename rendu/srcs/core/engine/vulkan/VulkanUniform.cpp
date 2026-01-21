#include "VulkanEngine.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <iostream>

void VulkanEngine::_createUniformBuffers(PipelineData & pipelineData)
{
	// On crée un uniform buffer par frame en vol (MAX_FRAMES_IN_FLIGHT)
	// pour éviter les conflits d’écriture entre le CPU (qui met à jour les uniforms)
	// et le GPU (qui lit ces données pendant le rendu).
	vk::DeviceSize size = 0;
	for (DescriptorInfo & descriptorInfo : pipelineData.pipelineInfo.descriptors)
	{
		if (descriptorInfo.type == DescriptorType::UNIFORM_BUFFER)
			size += descriptorInfo.size;
	}
	pipelineData.uniforms.size = size;
	if (size == 0)
		return;
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vk::raii::Buffer buffer = nullptr;
		vk::raii::DeviceMemory bufferMemory = nullptr;
		// On crée un buffer de type "uniform buffer" et on lui alloue de la mémoire visible par le CPU
		// (HOST_VISIBLE) pour pouvoir écrire directement dedans depuis l’application,
		// et "HOST_COHERENT" pour s’assurer que les écritures CPU soient immédiatement visibles du GPU
		// sans nécessiter d’appels explicites à vkFlushMappedMemoryRanges.
		_createBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, 
				buffer, bufferMemory);

		pipelineData.uniforms.bufferData.emplace_back(BufferData{std::move(buffer), std::move(bufferMemory)});

		// On mappe la mémoire du buffer pour obtenir un pointeur CPU accessible en écriture.
		// On garde ce pointeur persistant pendant tout le programme pour pouvoir mettre à jour
		// facilement le contenu du uniform buffer à chaque frame.
		pipelineData.uniforms.mapped.emplace_back(pipelineData.uniforms.bufferData[i].memory.mapMemory(0, size));
	}

	Logger::log(ENGINE_VULKAN, INFO, "Created Uniform Buffers.");
}

void VulkanEngine::updateUniformBuffer(PipelineID pipelineID, void * data, size_t size)
{
	memcpy(_pipelineMap[pipelineID].uniforms.mapped[_currentFrame], data, size);
}
