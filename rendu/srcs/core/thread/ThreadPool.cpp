#include "ThreadPool.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"
#include <thread>

uint16_t ThreadPool::_count = 0;

void ThreadPool::start(uint16_t totalThreads)
{
	_id = _count;
	if (_isStarted)
	{
		Logger::log(THREAD, WARNING, "Attempted to start ThreadPool #" + toString(_id) + " that's already started.");
		return;
	}
	unsigned int threadCount = std::thread::hardware_concurrency();
	if (totalThreads >= threadCount)
	{

		Logger::log(THREAD, WARNING, "Total threads exceed the host's thread count by " + toString(totalThreads - threadCount) +
			". Adjusting to " + toString(threadCount - 1) + " threads.");
		totalThreads = threadCount - 1;
	}

	for (int i = 0; i < totalThreads; ++i)
	{
		ThreadWorker * worker = new ThreadWorker();
		worker->start();
		_workers.push_back(worker);
	}
	_count++;
	_isStarted = true;
}

void ThreadPool::stop()
{
	_isStarted = false;
	for (ThreadWorker * worker : _workers)
	{
		worker->stop();
		delete worker;
	}
}
