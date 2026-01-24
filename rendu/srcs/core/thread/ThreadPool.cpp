#include "ThreadPool.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"
#include <thread>

uint16_t ThreadPool::_count = 0;
uint16_t ThreadPool::_availableThreads = std::thread::hardware_concurrency() - 1;

void ThreadPool::start(uint16_t requestedThreads)
{
	if (_availableThreads == 0 || _availableThreads - requestedThreads < 0)
		throw ThreadException("Can't instantiate ThreadPool #" + toString(_count) + ": No threads are available.");
	if (_isActive)
	{
		Logger::log(THREAD, WARNING, "Attempted to start ThreadPool #" + toString(_id) + " that's already active.");
		return;
	}
	_id = _count;

	if (requestedThreads > _availableThreads)
	{
		Logger::log(THREAD, WARNING, "Total threads exceed the host's thread count by " + toString(requestedThreads - _availableThreads) +
			". Adjusting to " + toString(_availableThreads) + " threads.");
		requestedThreads = _availableThreads;
	}
	_availableThreads -= requestedThreads;

	_isActive = true;
	for (int i = 0; i < requestedThreads; ++i)
	{
		std::unique_ptr<ThreadWorker> worker = std::make_unique<ThreadWorker>(_wakerCv, _queueMutex, _taskQueue, _isActive);
		worker->start();
		_workers.push_back(std::move(worker));
	}
	_count++;
}

void ThreadPool::submitTask(Task task)
{
	if (!_isActive)
	{
		Logger::log(THREAD, WARNING, "Attempted to submit a task to ThreadPool #" + toString(_id) + " before starting it.");
		return;
	}
	std::lock_guard<std::mutex> lock(_queueMutex);
	_taskQueue.push(std::move(task));
	_wakerCv.notify_one();
}

void ThreadPool::stop()
{
	_availableThreads += _workers.size();
	{
		std::lock_guard<std::mutex> lock(_queueMutex);
		_isActive = false;
		_wakerCv.notify_all();
	}
	for (std::unique_ptr<ThreadWorker> & worker : _workers)
		worker->stop();
}

void ThreadPool::giveBackThreads(uint16_t threadCount)
{
	uint16_t totalThreads = std::thread::hardware_concurrency() - 1;
	if (threadCount > totalThreads)
	{
		Logger::log(THREAD, WARNING, "Giving back threads exceed the host's thread count by " + toString(_availableThreads + threadCount - totalThreads) +
			". Adjusting to " + toString(totalThreads) + " threads.");
		_availableThreads = totalThreads;
	}
	else
		_availableThreads += threadCount;
}

void ThreadPool::takeFromThreads(uint16_t threadCount)
{
	if (threadCount > _availableThreads)
	{
		if (threadCount - _availableThreads <= 0)
			throw ThreadException("No threads available for take operation.");
		Logger::log(THREAD, WARNING, "Taking threads exceed the host's thread count by " + toString(threadCount - _availableThreads) +
			". Adjusting to " + toString(_availableThreads) + " threads.");
		threadCount = _availableThreads;
	}
	_availableThreads -= threadCount;
}
