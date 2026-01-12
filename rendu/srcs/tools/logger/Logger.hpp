#pragma once

#include <optional>
#include <string>
#include <iostream>

enum LogSeverity
{
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATAL
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
	PIPELINE,
	THREAD,
	PROFILER
};

typedef std::optional<std::ostream> OptionalOutputFile;

class Logger
{
	public:
		Logger() = delete;
		~Logger() = delete;

		static void	log(LogSource source, LogSeverity severity, const std::string & message, std::ostream * output = nullptr);

};
