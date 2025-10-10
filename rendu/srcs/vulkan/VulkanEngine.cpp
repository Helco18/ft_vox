#include "VulkanEngine.hpp"

VulkanEngine::VulkanEngine(GLFWwindow * window): _window(window)
{
	_createInstance();
	_initDebugMessenger();
	_createSurface();
	_selectPhysicalDevice();
	_checkDeviceExtensions();
	_createLogicalDevice();
	_createSwapChain();
	_createImageViews();
	_createGraphicsPipeline();
	_createCommandPool();
	_createCommandBuffer();
	_createSyncObjects();
}

VulkanEngine::~VulkanEngine() {}

void VulkanEngine::drawFrame()
{
	_queue.waitIdle();

	while (vk::Result::eTimeout == _device.waitForFences(*_inFlightFences[_currentFrame], vk::True, std::numeric_limits<uint64_t>::max()))
		;

	// Semaphore = on ordonne les tâches
	// Fence = on attend que le GPU finisse la tâche
	try
	{
		std::pair<vk::Result, uint32_t> result = _swapChain.acquireNextImage(std::numeric_limits<uint64_t>::max(), *_presentCompleteSemaphores[_semaphoreIndex], nullptr);
		if (result.first != vk::Result::eSuccess)
		{
			if (result.first == vk::Result::eErrorOutOfDateKHR)
			{
				_recreateSwapchain();
				return;
			}
			throw std::runtime_error("Couldn't acquire next image.");
		}
		_device.resetFences(*_inFlightFences[_currentFrame]);

		_commandBuffers[_currentFrame].reset();
		_recordCommandBuffer(result.second);

		vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &*_presentCompleteSemaphores[_semaphoreIndex];
		submitInfo.pWaitDstStageMask = &waitDstStageMask;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &*_commandBuffers[_currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &*_renderFinishedSemaphores[_currentFrame];

		_queue.submit(submitInfo, *_inFlightFences[_currentFrame]);

		vk::PresentInfoKHR presentInfoKHR;
		presentInfoKHR.waitSemaphoreCount = 1;
		presentInfoKHR.pWaitSemaphores = &*_renderFinishedSemaphores[_currentFrame];
		presentInfoKHR.swapchainCount = 1;
		presentInfoKHR.pSwapchains = &*_swapChain;
		presentInfoKHR.pImageIndices = &result.second;

		vk::Result presentResult = _queue.presentKHR(presentInfoKHR);

		if (presentResult != vk::Result::eSuccess)
			throw std::runtime_error("Couldn't present next image.");

		_semaphoreIndex = (_semaphoreIndex + 1) % _presentCompleteSemaphores.size();
		_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	} catch (const vk::OutOfDateKHRError & e)
	{
		_framebufferResized = false;
		_recreateSwapchain();
	}
}
