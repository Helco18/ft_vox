#pragma once

#include "ThreadWorker.hpp"
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <queue>

typedef std::function<void()>	Task;

class ThreadPool
{
	public:
		ThreadPool() {};
		~ThreadPool() {};

		void					start(uint16_t totalThreads);
		void					stop();
		void					submitTask(Task task); // use Task task = [params]() { func(); }; to send it.
		
		static unsigned int		getHostThreadCount();

	private:
		typedef std::vector<std::unique_ptr<ThreadWorker>> WorkerPool;

		static uint16_t			_count;
		uint16_t				_id;
		std::queue<Task>		_taskQueue;
		std::condition_variable _wakerCv;
		std::mutex 				_queueMutex;
		WorkerPool				_workers;
		std::atomic_bool		_isActive = false;
};
