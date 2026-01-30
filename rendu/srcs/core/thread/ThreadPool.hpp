#pragma once

#include "ThreadWorker.hpp"

typedef std::function<void()>	Task;

class ThreadPool
{
	public:
		ThreadPool() {};
		~ThreadPool() {};

		void					start(uint16_t requestedThreads);
		void					stop();
		void					submitTask(Task task); // use Task task = [params]() { func(params); }; to send it.

		static uint16_t			getAvailableThreads() { return _availableThreads; }
		static void				giveBackThreads(uint16_t threadCount);
		static void				takeFromThreads(uint16_t threadCount);
		
	private:
		typedef std::unordered_map<uint16_t, std::vector<std::unique_ptr<ThreadWorker>>> WorkerPoolMap;

		static uint16_t			_count;
		static uint16_t			_availableThreads;
		uint16_t				_id;
		std::queue<Task>		_taskQueue;
		std::condition_variable _wakerCv;
		std::mutex 				_queueMutex;
		WorkerPoolMap			_workers;
		uint16_t				_threadCount;
		std::atomic_bool		_isActive = false;
		bool					_isStopped = false;
};
