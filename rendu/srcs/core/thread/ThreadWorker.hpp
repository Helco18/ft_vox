#pragma once

#include <atomic>
#include <thread>
#include <chrono>

#define USLEEP(x) std::this_thread::sleep_for(std::chrono::microseconds(x))

class ThreadWorker
{
	public:
		ThreadWorker() {};
		~ThreadWorker() {};

		void				start();
		void				stop();

	private:
		void				_process();
		void				_loop();

		static uint16_t		_count;
		uint16_t			_id;
		std::thread			_thread;
		std::atomic<bool>	_isWorking = false;
		std::atomic<bool>	_isStarted = false;
};
