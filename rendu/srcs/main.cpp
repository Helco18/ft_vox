#include "Environment.hpp"
#include "CustomExceptions.hpp"
#include "Profiler.hpp"
#include <algorithm>
#include <vulkan/vulkan.hpp>

static ProgramParams parseArgs(int ac, char ** av)
{
	ProgramParams params;
	std::vector<std::string> args(&av[1], &av[1] + (ac - 1));
	char paramMode = 0;

	for (std::string & str : args)
	{
		size_t strSize = str.size();
		if (paramMode)
		{
			if (str.empty())
				throw GeneralException("Specify a value for parameter of type '" + toString(paramMode) + "'.");
			int32_t value = std::atof(str.c_str());
			switch (paramMode)
			{
				case 'r': {
					if (strSize > 2 || value > 64 || value == 0 || !std::all_of(str.begin(), str.end(), [](const char c) { return std::isdigit(c); }))
						throw GeneralException("Render distance of '" + str + "' is invalid. It must only contain numbers and be between 1 and 64.");
					params.renderDistance = value;
				} break;
				case 'f': {
					if (strSize > 3 || value > 180 || value == 0 || !std::all_of(str.begin(), str.end(), [](const char c) { return std::isdigit(c); }))
						throw GeneralException("FOV of '" + str + "' is invalid. It must only contain numbers and be between 1 and 180.");
					params.fov = value;
				} break;
				case 's': {
					if (!std::all_of(str.begin(), str.end(), [](const char c) { return std::isdigit(c); }))
						throw GeneralException("Seed of '" + str + "' is invalid. It must only contain numbers.");
					params.seed = value;
				} break;
				case 'x': {
					if (!std::all_of(str.begin() + (str[0] == '-'), str.end(), [](const char c) { return std::isdigit(c); }))
						throw GeneralException("Coordinate X '" + str + "' is invalid. It must only contain numbers.");
					params.spawnLocation.x = value;
				} break;
				case 'y': {
					if (!std::all_of(str.begin() + (str[0] == '-'), str.end(), [](const char c) { return std::isdigit(c); }))
						throw GeneralException("Coordinate Y '" + str + "' is invalid. It must only contain numbers.");
					params.spawnLocation.y = value;
				} break;
				case 'z': {
					if (!std::all_of(str.begin() + (str[0] == '-'), str.end(), [](const char c) { return std::isdigit(c); }))
						throw GeneralException("Coordinate Z '" + str + "' is invalid. It must only contain numbers.");
					params.spawnLocation.z = value;
				} break;
			}
			paramMode = 0;
		}
		else if (str[0] == '-' && strSize > 1)
		{
			for (int i = 1; str[i]; ++i)
			{
				if (str[i] == 'd')
					g_debug = DebugLevel::DEBUG_MESSAGES;
				else if (str[i] == 'v' && strcmp(&str[i], "vsync"))
					g_debug = DebugLevel::VALIDATION;
				else if (str[i] == 'p')
					Profiler::enable();
				else if (str[i] == 'r' || str[i] == 's' || str[i] == 'x' || str[i] == 'y' || str[i] == 'z')
					paramMode = str[i];
				else if (!strcmp(&str[i], "fov"))
				{
					paramMode = 'f';
					break;
				}
				else if (!strcmp(&str[i], "opengl") || !strcmp(&str[i], "gl"))
				{
					params.engineType = OPENGL;
					break;
				}
				else if (!strcmp(&str[i], "vsync"))
				{
					params.vsync = true;
					break;
				}
				else
					throw GeneralException("Invalid argument: '" + toString(str) + "'.");
			}
		}
		else
			throw GeneralException("Invalid argument: '" + str + "'.");
	}
	if (paramMode)
		throw GeneralException("Specify a value for parameter of type '" + toString(paramMode) + "'.");
	return params;
}

int main(int ac, char ** av)
{
	Environment environment;
	ProgramParams params;

	try
	{
		if (ac > 1)
			params = parseArgs(ac, av);

		environment.init(params);
		environment.loop();

		Logger::log(GENERAL, INFO, "Exiting program.");
	}
	catch (const CustomException & e)
	{
		Logger::log(e.getSource(), FATAL, std::string(e.what()));
		return 1;
	}
	catch (const vk::OutOfDeviceMemoryError & e)
	{
		Logger::log(ENGINE_VULKAN, FATAL, std::string(e.what()));
		return 1;
	}
	Profiler::print();
	return EXIT_SUCCESS;
}
