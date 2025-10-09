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
	// Semaphore = on ordonne les tâches
	// Fence = on attend que le GPU finisse la tâche
	 
	std::pair<vk::Result, uint32_t> result = _swapChain.acquireNextImage(std::numeric_limits<uint64_t>::max(), *_presentCompleteSemaphore, nullptr);
	if (result.first != vk::Result::eSuccess)
		throw std::runtime_error("Couldn't acquire next image.");
	_recordCommandBuffer(result.second);
	_device.resetFences(*_drawFence);

	vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::SubmitInfo submitInfo;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &*_presentCompleteSemaphore;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &*_commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &*_renderFinishedSemaphore;

	_queue.submit(submitInfo, *_drawFence);
	while (vk::Result::eTimeout == _device.waitForFences(*_drawFence, vk::True, std::numeric_limits<uint64_t>::max()))
		;

	vk::PresentInfoKHR presentInfoKHR;
	presentInfoKHR.waitSemaphoreCount = 1;
	presentInfoKHR.pWaitSemaphores = &*_renderFinishedSemaphore;
	presentInfoKHR.swapchainCount = 1;
	presentInfoKHR.pSwapchains = &*_swapChain;
	presentInfoKHR.pImageIndices = &result.second;

	vk::Result presentResult = _queue.presentKHR(presentInfoKHR);
	if (presentResult != vk::Result::eSuccess)
		throw std::runtime_error("Couldn't present next image.");
}

void VulkanEngine::waitIdle()
{
	_device.waitIdle();
}
