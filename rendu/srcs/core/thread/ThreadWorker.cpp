#include "ThreadWorker.hpp"
#include "Logger.hpp"
#include "utils.hpp"

uint16_t ThreadWorker::_count = 0;

void ThreadWorker::start()
{
	_id = _count;
	if (_isStarted)
	{
		Logger::log(THREAD, WARNING, "Attempted to start ThreadWorker #" + toString(_id) + " that's already started.");
		return;
	}
	_thread = std::thread(&ThreadWorker::_loop, this);
	_count++;
	_isStarted = true;
}

void ThreadWorker::_loop()
{
	while (_isStarted)
	{
		_process();
		USLEEP(100);
	}
}

void ThreadWorker::_process()
{
	// task
}

void ThreadWorker::stop()
{
	_isStarted = false;
	if (_thread.joinable())
		_thread.join();
}
