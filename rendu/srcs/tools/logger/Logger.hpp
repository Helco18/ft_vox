#pragma once

#include <optional>
#include <string>

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

typedef std::optional<std::reference_wrapper<std::ostream>> OptionalOutputFile;

class Logger
{
	public:
		Logger() = delete;
		~Logger() = delete;

		static void	log(LogSource source, LogSeverity severity, const std::string & message, OptionalOutputFile output = std::nullopt);

};
