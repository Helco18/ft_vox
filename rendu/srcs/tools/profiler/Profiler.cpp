#include "Profiler.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <fstream>
#include <algorithm>
#ifdef __has_include
	#if __has_include(<valgrind/valgrind.h>)
		#include <valgrind/valgrind.h>
		#define VALGRIND_AVAILABLE
	#endif
#endif

Profiler::ProfileMap Profiler::_profileCache;
std::mutex Profiler::_cacheMutex;
bool Profiler::_isEnabled = false;

inline static MicroTime getMicroTime()
{
	return std::chrono::duration_cast<MicroTime>(std::chrono::high_resolution_clock().now().time_since_epoch());
}

static std::string getFormattedTime(MicroTime microTime)
{
	std::ostringstream oss;

	oss << std::fixed << std::setprecision(2) << (microTime.count() / 1000.0) << " ms" << " (" << (microTime.count() / 1000000.0) << " s).";
	return oss.str();
}

static std::string getAverageTime(std::deque<MicroTime> & microTimes)
{
	MicroTime average;

	if (microTimes.empty())
		return getFormattedTime(MicroTime(0));
	else if (microTimes.size() == 1)
		return getFormattedTime(microTimes[0]);

	// this adds all microTimes and uses as its initializer a duration of 0 microseconds stored as a double (to allow for bigger numbers)
	average = std::chrono::duration_cast<MicroTime>(std::accumulate(
		microTimes.begin(), microTimes.end(),
		std::chrono::duration<double, std::micro>(0)) / microTimes.size()
	);
	return getFormattedTime(MicroTime(average));
}

static std::string getMedianTime(std::deque<MicroTime> & microTimes)
{
	MicroTime median;

	if (microTimes.empty())
		return getFormattedTime(MicroTime(0));
	else if (microTimes.size() == 1)
		return getFormattedTime(microTimes[0]);

	std::sort(microTimes.begin(), microTimes.end());
	median = microTimes[microTimes.size() / 2 - 1];
	return getFormattedTime(MicroTime(median));
}

// Thank you CPP00 ex02
static std::string getFilename()
{
	std::ostringstream oss;

	char	buffer[30];
	time_t now = std::time(NULL);
	struct tm *time = std::localtime(&now);
	std::strftime(buffer, 29, "profiler_%Y%m%d_%H%M%S.txt", time);
	oss << buffer;
	return oss.str();
}

Profiler::Profiler(const std::string & name): _name(name)
{
	_startTime = getMicroTime();
}

void Profiler::print()
{
	if (!_isEnabled)
		return;
	std::string filename;
	std::filesystem::path filepath = LOGFILE_DIR;
	std::fstream file;

	std::lock_guard<std::mutex> lg(_cacheMutex);
	if (_profileCache.empty())
		return;
	filename = LOGFILE_DIR + getFilename();
	std::filesystem::create_directories(filepath.parent_path());
	file.open(filename, std::ios::out);
	if (file.fail())
	{
		Logger::log(PROFILER, ERROR, "Couldn't create profiler file: " + filename, &file);
		return;
	}
	#ifdef VALGRIND_AVAILABLE
	if (RUNNING_ON_VALGRIND)
	{
		Logger::log(PROFILER, INFO, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", &file);
		Logger::log(PROFILER, INFO, "!Warning: Valgrind was used. Timing won't be exact.!", &file);
		Logger::log(PROFILER, INFO, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", &file);
	}
	#endif
	for (std::pair<const std::string, Profile> & profiles : _profileCache)
	{
		Profile & profile = profiles.second;
		std::string name = profiles.first;
		
		Logger::log(PROFILER, INFO, "Profile for: " + name + ":", &file);
		Logger::log(PROFILER, INFO, "First execution time: " + getFormattedTime(profile.firstExecTime), &file);
		Logger::log(PROFILER, INFO, "Last execution time: " + getFormattedTime(profile.lastExecTime), &file);
		Logger::log(PROFILER, INFO, "Slowest execution time: " + getFormattedTime(profile.slowestExecTime), &file);
		Logger::log(PROFILER, INFO, "Fastest execution time: " + getFormattedTime(profile.fastestExecTime), &file);
		Logger::log(PROFILER, INFO, "Average execution time: " + getAverageTime(profile.recordedTimes), &file);
		Logger::log(PROFILER, INFO, "Median execution time: " + getMedianTime(profile.recordedTimes), &file);
		Logger::log(PROFILER, INFO, "Number of executions: " + toString(profile.times), &file);
		Logger::log(PROFILER, INFO, "--------------------------------", &file);
	}
	Logger::log(PROFILER, INFO, "Logged output in: " + filename);
}

void Profiler::stop()
{
	if (!_isEnabled)
		return;
	std::lock_guard<std::mutex> lg(_cacheMutex);
	MicroTime execTime;
	Profile & profile = _profileCache[_name];
	
	execTime = getMicroTime() - _startTime;
	if (profile.recordedTimes.empty())
	{
		profile.times = 1;
		profile.firstExecTime = execTime;
		profile.slowestExecTime = execTime;
		profile.fastestExecTime = execTime;
		profile.name = _name;
	}
	else
		profile.times++;
	profile.lastExecTime = execTime;
	if (execTime > profile.slowestExecTime)
		profile.slowestExecTime = execTime;
	else if (execTime < profile.fastestExecTime)
		profile.fastestExecTime = execTime;
	profile.recordedTimes.push_back(execTime);
	if (profile.recordedTimes.size() >= MAX_RECORDED_TIMES)
		profile.recordedTimes.pop_front();
	_earlyStop.store(true);
}

Profiler::~Profiler()
{
	if (!_earlyStop.load())
		stop();
}
