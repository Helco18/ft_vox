#include "ThreadPool.hpp"
#include "Logger.hpp"

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
			Logger::log(THREAD, DEBUG, "Philosopher is sleeping");
		{
			std::unique_lock<std::mutex> lock(_queueMutex);
			_wakerCv.wait(lock, [&] { return !_isActive.load() || !_taskQueue.empty(); });

			Logger::log(THREAD, DEBUG, "Philosopher is thinking");
			if (!_isActive.load())
			{
				Logger::log(THREAD, DEBUG, "Philosopher died");
				_queueMutex.unlock();
				return;
			}

			Logger::log(THREAD, DEBUG, "Philosopher is eating");
			task = std::move(_taskQueue.front());
			_taskQueue.pop();
		}
		if (task)
			task();
	}
}

void ThreadWorker::stop()
{
	if (_thread.joinable())
	{
		Logger::log(THREAD, DEBUG, "Joining worker");
		_thread.join();
		Logger::log(THREAD, DEBUG, "Worker joined");
	}
}
