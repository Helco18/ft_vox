#include "ThreadPool.hpp"
#include "Logger.hpp"
#include "utils.hpp"

uint16_t ThreadWorker::_count = 0;

void ThreadWorker::start()
{
	_id = _count;
	_thread = std::thread(&ThreadWorker::_loop, this);
	_count++;
}

void ThreadWorker::_loop()
{
	while (_isActive)
	{
		Task task;
		{
			std::unique_lock<std::mutex> lock(_queueMutex);
			_wakerCv.wait(lock, [&] { return !_isActive || !_taskQueue.empty(); });

			if (_isActive)
			{
				task = std::move(_taskQueue.front());
				_taskQueue.pop();
			}
		}
		if (task)
			task();
	}
}

void ThreadWorker::stop()
{
	if (_thread.joinable())
		_thread.join();
}
