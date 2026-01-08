#pragma once

#include <string>

enum LogSeverity
{
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	CRITICAL
};

enum LogSource
{
	ENGINE_OPENGL,
	ENGINE_VULKAN,
	VOXEL,
	ENVIRONMENT,
	INPUT,
	MODEL,
	SHADER,
	WINDOW,
	GENERAL,
	TEXTURE,
	PIPELINE
};

class Logger
{
	public:
		Logger() = delete;
		~Logger() = delete;

		static void					log(LogSource source, LogSeverity severity, const std::string & message);

	private:
		static const std::string	_getLogSeverityPrefix(LogSeverity severity);
		static const std::string	_getLogSourcePrefix(LogSource severity);
};
