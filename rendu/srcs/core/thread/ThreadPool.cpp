#include "ThreadPool.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"
#include <thread>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

uint16_t ThreadPool::_count = 0;
uint16_t ThreadPool::_availableThreads = 0;

unsigned int ThreadPool::getHostThreadCount()
{
	unsigned int threadCount = 1;

	#ifdef _WIN32
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	threadCount = info.dwNumberOfProcessors;
	#elif defined(__APPLE__)
	size_t size = sizeof(threadCount);
	sysctlbyname("hw.logicalcpu", &threadCount, &size, nullptr, 0);
	#elif defined(__linux__)
	threadCount = sysconf(_SC_NPROCESSORS_ONLN);
	#endif

	return threadCount;
}

void ThreadPool::start(uint16_t requestedThreads)
{
	_id = _count;
	if (_isActive)
	{
		Logger::log(THREAD, WARNING, "Attempted to start ThreadPool #" + toString(_id) + " that's already active.");
		return;
	}

	if (_availableThreads == 0)
		_availableThreads = getHostThreadCount();
	if (requestedThreads >= _availableThreads)
	{
		Logger::log(THREAD, WARNING, "Total threads exceed the host's thread count by " + toString(requestedThreads - _availableThreads + 1) +
			". Adjusting to " + toString(_availableThreads - 1) + " threads.");
		requestedThreads = _availableThreads - 1;
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
