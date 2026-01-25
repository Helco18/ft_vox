#include "VulkanThread.hpp"
#include "CustomExceptions.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include "ThreadPool.hpp"

void VulkanThread::start(uint16_t threadCount, vk::raii::Device & device, uint32_t queueIndex)
{
	_threadCount = threadCount;
	ThreadPool::takeFromThreads(_threadCount);
	std::lock_guard<std::mutex> lg(_commandMutex);

	for (uint16_t i = 0; i < _threadCount; ++i)
	{
		vk::CommandPoolCreateInfo createInfo;
		createInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		createInfo.queueFamilyIndex = queueIndex;
		_commandPools.push_back(vk::raii::CommandPool(device, createInfo));

		vk::CommandBufferAllocateInfo bufferInfo;
		bufferInfo.commandPool = _commandPools[i];
		bufferInfo.commandBufferCount = 1;
		bufferInfo.level = vk::CommandBufferLevel::ePrimary;

		_commandBuffers.push_back(device.allocateCommandBuffers(bufferInfo)[0]);
	}
	Logger::log(ENGINE_VULKAN, INFO, "Created " + toString(_threadCount) + " threads.");
}

std::vector<vk::raii::CommandBuffer> & VulkanThread::getWorkersBuffers()
{
	std::lock_guard<std::mutex> lg(_commandMutex);
	return _commandBuffers;
}

void VulkanThread::stop()
{
	ThreadPool::giveBackThreads(_threadCount);
	std::lock_guard<std::mutex> lg(_commandMutex);

}
