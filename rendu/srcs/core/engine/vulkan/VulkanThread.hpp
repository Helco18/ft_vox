#pragma once

#include "VulkanEngine.hpp"
#include <mutex>
#include <condition_variable>

class VulkanThread
{
	public:
		VulkanThread() {};
		~VulkanThread() {};

		void									start(uint16_t threadCount, vk::raii::Device & device, uint32_t queueIndex);
		void									queue(PendingAsset & pendingAsset);
		std::vector<vk::raii::CommandBuffer> &	getWorkersBuffers();
		void									stop();

	private:
		uint16_t								_threadCount = 0;
		std::mutex								_commandMutex;
		std::vector<vk::raii::CommandPool>		_commandPools;
		std::vector<vk::raii::CommandBuffer>	_commandBuffers;
		std::vector<PendingAsset>				_queue;
		// std::vector<VulkanWorker>				_workers;
};
