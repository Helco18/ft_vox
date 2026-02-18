#include "VulkanEngine.hpp"
#include "Logger.hpp"
#include "utils.hpp"

void VulkanEngine::_createUniformBuffers(PipelineData & pipelineData)
{
	// On crée un uniform buffer par frame en vol (MAX_FRAMES_IN_FLIGHT)
	// pour éviter les conflits d’écriture entre le CPU (qui met à jour les uniforms)
	// et le GPU (qui lit ces données pendant le rendu).
	for (DescriptorInfo & descriptorInfo : pipelineData.pipelineInfo.descriptors)
	{
		if (descriptorInfo.type != DescriptorType::UNIFORM_BUFFER)
			continue;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			UniformBufferData & uniformBufferData = pipelineData.uniforms[descriptorInfo.binding];
			vk::DeviceSize size = descriptorInfo.size;
			uniformBufferData.size = size;
			vk::raii::Buffer buffer = nullptr;
			vk::raii::DeviceMemory bufferMemory = nullptr;
			// On crée un buffer de type "uniform buffer" et on lui alloue de la mémoire visible par le CPU
			// (HOST_VISIBLE) pour pouvoir écrire directement dedans depuis l’application,
			// et "HOST_COHERENT" pour s’assurer que les écritures CPU soient immédiatement visibles du GPU
			// sans nécessiter d’appels explicites à vkFlushMappedMemoryRanges.
			_createBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer,
					vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, 
					buffer, bufferMemory);

			uniformBufferData.bufferData.emplace_back(BufferData{std::move(buffer), std::move(bufferMemory)});

			// On mappe la mémoire du buffer pour obtenir un pointeur CPU accessible en écriture.
			// On garde ce pointeur persistant pendant tout le programme pour pouvoir mettre à jour
			// facilement le contenu du uniform buffer à chaque frame.
			uniformBufferData.mapped.emplace_back(uniformBufferData.bufferData[i].memory.mapMemory(0, size));
		}
	}
	Logger::log(ENGINE_VULKAN, INFO, "Created Uniform Buffers.");
}

void VulkanEngine::updateUniformBuffer(PipelineID pipelineID, unsigned int binding, void * data, size_t size)
{
	PendingUniform uniform;
	uniform.pipelineID = pipelineID;
	uniform.binding = binding;
	uniform.data = data;
	uniform.size = size;
	_pendingUniforms.push_back(uniform);
}

void VulkanEngine::_processPendingUniforms()
{
	if (_pendingUniforms.empty())
		return;
	for (PendingUniform & uniform : _pendingUniforms)
	{
		PipelineData & pipelineData = _pipelineCache[uniform.pipelineID];
		std::unordered_map<unsigned int, UniformBufferData>::iterator it = pipelineData.uniforms.find(uniform.binding);
		if (it == pipelineData.uniforms.end())
		{
			Logger::log(ENGINE_VULKAN, WARNING, "Tried to process uniform buffer to unknown binding ID #" + toString(uniform.binding));
			continue;
		}
		UniformBufferData & uniformBufferData = it->second;
		memcpy(uniformBufferData.mapped[_currentFrame], uniform.data, uniform.size);
	}
	_pendingUniforms.clear();
}
