#pragma once

#include "ThreadWorker.hpp"
#include <vector>
#include <cstdint>

class ThreadPool
{
	public:
		ThreadPool() {};
		~ThreadPool() {};

		void			start(uint16_t totalThreads);
		void			stop();

	private:
		typedef std::vector<ThreadWorker *> WorkerPool;

		static uint16_t	_count;
		uint16_t		_id;
		WorkerPool		_workers;
		bool			_isStarted = false;
};
