#include "VulkanEngine.hpp"
#include "Logger.hpp"

void VulkanEngine::_createSyncObjects()
{
	_presentCompleteSemaphores.clear();
	_renderFinishedSemaphores.clear();
	_inFlightFences.clear();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vk::FenceCreateInfo fenceInfo;
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
		_inFlightFences.emplace_back(_device, fenceInfo);
		_presentCompleteSemaphores.emplace_back(_device, vk::SemaphoreCreateInfo());
	}
	for (size_t i = 0; i < _swapChainImages.size(); ++i)
		_renderFinishedSemaphores.emplace_back(_device, vk::SemaphoreCreateInfo());
	Logger::log(ENGINE_VULKAN, INFO, "Created Sync Objects.");
}
