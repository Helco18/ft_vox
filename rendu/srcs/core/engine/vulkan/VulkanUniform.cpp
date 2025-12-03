#include "VulkanEngine.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Logger.hpp"
#include <iostream>

void VulkanEngine::_createUniformBuffers()
{
	_uniformBuffers.clear();
	_uniformBuffersMemory.clear();
	_uniformBuffersMapped.clear();

	// On crée un uniform buffer par frame en vol (MAX_FRAMES_IN_FLIGHT)
	// pour éviter les conflits d’écriture entre le CPU (qui met à jour les uniforms)
	// et le GPU (qui lit ces données pendant le rendu).
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vk::DeviceSize size = sizeof(UniformBuffer);
		vk::raii::Buffer buffer = nullptr;
		vk::raii::DeviceMemory bufferMemory = nullptr;

		// On crée un buffer de type "uniform buffer" et on lui alloue de la mémoire visible par le CPU
		// (HOST_VISIBLE) pour pouvoir écrire directement dedans depuis l’application,
		// et "HOST_COHERENT" pour s’assurer que les écritures CPU soient immédiatement visibles du GPU
		// sans nécessiter d’appels explicites à vkFlushMappedMemoryRanges.
		_createBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, buffer, bufferMemory);

		// On mappe la mémoire du buffer pour obtenir un pointeur CPU accessible en écriture.
		// On garde ce pointeur persistant pendant tout le programme pour pouvoir mettre à jour
		// facilement le contenu du uniform buffer à chaque frame.
		_uniformBuffers.emplace_back(std::move(buffer));
		_uniformBuffersMemory.emplace_back(std::move(bufferMemory));
		_uniformBuffersMapped.emplace_back(_uniformBuffersMemory[i].mapMemory(0, size));
	}

	if (g_enableValidationLayers)
		Logger::log(ENGINE_VULKAN, INFO, "Created Uniform Buffers.");
}

void VulkanEngine::_updateUniformBuffer()
{
	UniformBuffer ubo{};

	// Camera orientation (Y-up)
	glm::vec3 camPos = _camera->getPosition();

	glm::vec3 forward = _camera->computeForward();

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	ubo.view = glm::lookAt(camPos, camPos + forward, up);

	// Vulkan projection (flip Y)
	ubo.proj = glm::perspective(glm::radians(_camera->getFOV()),
		static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height),
		0.01f * NEAR_PLANE_OFFSET, 1500.0f);
	ubo.proj[1][1] *= -1;

	memcpy(_uniformBuffersMapped[_currentFrame], &ubo, sizeof(ubo));
}
