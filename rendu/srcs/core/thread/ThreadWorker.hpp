#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <thread>
#include <chrono>
#include <queue>

#define USLEEP(x) std::this_thread::sleep_for(std::chrono::microseconds(x))

typedef std::function<void()>	Task;

class ThreadWorker
{
	public:
		ThreadWorker(std::condition_variable & wakerCv, std::mutex & queueMutex, std::queue<Task> & taskQueue, std::atomic_bool & isActive) :
			_wakerCv(wakerCv), _queueMutex(queueMutex), _taskQueue(taskQueue), _isActive(isActive) {};
		~ThreadWorker() {};

		void						start();
		void						stop();

	private:
		void						_loop();

		static uint16_t				_count;
		uint16_t					_id;
		std::thread					_thread;
		std::condition_variable &	_wakerCv;
		std::mutex &				_queueMutex;
		std::queue<Task> &			_taskQueue;
		std::atomic<bool> &			_isActive;
};
