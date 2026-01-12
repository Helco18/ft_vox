#include "Logger.hpp"
#include "colors.hpp"
#include <iomanip>
#include <iostream>
#include <ctime>
#include <chrono>
#include <optional>
#include <sstream>

static const std::string getTimestampAsDate()
{
	std::ostringstream oss;
	std::chrono::time_point now = std::chrono::system_clock::now();

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
	time_t timeTNow = std::chrono::system_clock::to_time_t(now);
	struct tm * sTime = std::localtime(&timeTNow);
	oss << std::put_time(sTime, "[%Y/%m/%d %I:%M:%S");
	oss << "." << std::setw(3) << std::setfill('0') << ms.count();
	oss << " " << std::put_time(sTime, "%p] ");

	return oss.str();
}

static const std::string getLogSeverityPrefixColor(LogSeverity severity)
{
	switch (static_cast<int>(severity))
	{
		case DEBUG: return MAGENTA"[DEBUG]";
		case INFO: return GRAY"[INFO]";
		case WARNING: return YELLOW"[WARNING]";
		case ERROR: return RED"[ERROR]";
		case FATAL: return BOLD_RED"[FATAL]";
		default: return "[UNKNOWN]";
	}
}

static const std::string getLogSeverityPrefix(LogSeverity severity)
{
	switch (static_cast<int>(severity))
	{
		case DEBUG: return "[DEBUG]";
		case INFO: return "[INFO]";
		case WARNING: return "[WARNING]";
		case ERROR: return "[ERROR]";
		case FATAL: return "[FATAL]";
		default: return "[UNKNOWN]";
	}
}

static const std::string getLogSourcePrefix(LogSource source)
{
	switch (static_cast<int>(source))
	{
		case ENGINE_OPENGL: return "OPENGL";
		case ENGINE_VULKAN: return "VULKAN";
		case VOXEL: return "VOXEL";
		case ENVIRONMENT: return "ENVIRONMENT";
		case INPUT: return "INPUT";
		case MODEL: return "MODEL";
		case SHADER: return "SHADER";
		case WINDOW: return "WINDOW";
		case GENERAL: return "GENERAL";
		case TEXTURE: return "TEXTURE";
		case PIPELINE: return "PIPELINE";
		case THREAD: return "THREAD";
		case PROFILER: return "PROFILER";
		default: return "UNKNOWN";
	}
}

void Logger::log(LogSource source, LogSeverity severity, const std::string & message, std::ostream * output)
{
	std::ostream & outputStream = severity >= ERROR ? std::cerr : output == nullptr ? std::cout : *output;
	outputStream
		<< getTimestampAsDate()
		<< (output == nullptr ? getLogSeverityPrefixColor(severity) : getLogSeverityPrefix(severity))
		<< '\t'
		<< getLogSourcePrefix(source)
		<< ": "
		<< message
		<< (output == nullptr ? RESET : "") << std::endl; 
}
