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
	while (true)
	{
		Task task;
		{
			std::unique_lock<std::mutex> lock(_queueMutex);
			_wakerCv.wait(lock, [&] { return !_isActive.load() || !_taskQueue.empty(); });

			if (!_isActive.load())
				return;

			task = std::move(_taskQueue.front());
			_taskQueue.pop_front();
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
