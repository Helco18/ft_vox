#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <chrono>
#include <deque>
#include <unordered_map>

#define LOGFILE_DIR "logs/"
#define MAX_RECORDED_TIMES 1000

typedef std::chrono::microseconds MicroTime;

struct Profile
{
	std::string				name;
	uint16_t				times;
	MicroTime				firstExecTime;
	MicroTime				lastExecTime;
	MicroTime				slowestExecTime;
	MicroTime				fastestExecTime;
	std::deque<MicroTime>	recordedTimes;
};

class Profiler
{
	public:
		Profiler(const std::string & name);
		~Profiler();

		void				stop();
		static void			print();
	
	private:
		typedef std::unordered_map<std::string, Profile>	ProfileMap;

		static ProfileMap	_profileCache;
		static std::mutex	_cacheMutex;
		std::string			_name;
		MicroTime			_startTime;
		std::atomic_bool	_earlyStop = false;
};
