#include "Logger.hpp"
#include "colors.hpp"
#include <iostream>

void Logger::log(LogSource source, LogSeverity severity, const std::string & message)
{
	std::ostream * outputStream = severity >= ERROR ? &std::cerr : &std::cout;
	*outputStream << _getLogSeverityPrefix(severity) << " " << _getLogSourcePrefix(source) << ": " << message << RESET << std::endl; 
}

const std::string Logger::_getLogSeverityPrefix(LogSeverity severity)
{
	switch (static_cast<int>(severity))
	{
		case DEBUG: return MAGENTA"[DEBUG]";
		case INFO: return GRAY"[INFO]";
		case WARNING: return YELLOW"[WARNING]";
		case ERROR: return RED"[ERROR]";
		case CRITICAL: return BOLD_RED"[CRITICAL]";
		default: return "UNKNOWN";
	}
}

const std::string Logger::_getLogSourcePrefix(LogSource source)
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
		default: return "UNKNOWN";
	}
}
