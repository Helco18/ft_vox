#include "VulkanEngine.hpp"

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
		std::cout << GREEN << "[OK] Created Uniform Buffers" << RESET << std::endl;
}

void VulkanEngine::_updateUniformBuffer()
{
	static std::chrono::time_point startTime = std::chrono::high_resolution_clock::now();
	std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBuffer ubo;
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height),
			0.1f, 10.0f);
	ubo.proj[1][1] *= -1; // Le Y est inversé ici

	// On copie notre uniform buffer dans la mémoire qui lui est réservée
	memcpy(_uniformBuffersMapped[_currentFrame], &ubo, sizeof(ubo));
}
