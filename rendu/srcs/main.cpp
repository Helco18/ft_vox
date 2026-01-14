#include "InputManager.hpp"
#include "OBJModel.hpp"
#include "Environment.hpp"
#include "CustomExceptions.hpp"
#include "Profiler.hpp"
#include "VulkanEngine.hpp"
#include <iostream>

static void parseArgs(int ac, char ** av, EngineType * engineType)
{
	std::vector<std::string> args(&av[1], &av[1] + (ac - 1));
	for (std::string & str : args)
	{
		if (str[0] == '-' && str.size() > 1)
		{
			for (int i = 1; str[i]; ++i)
			{
				if (str[i] == 'd')
					g_debug = true;
				else if (str[i] == 'p')
					Profiler::enable();
			}
		}
		else if (str == "gl" || str == "opengl")
			*engineType = OPENGL;
	}
}

int main(int ac, char ** av)
{
	EngineType engineType = VULKAN;
	Environment environment;

	if (ac > 1)
		parseArgs(ac, av, &engineType);

	try
	{
		environment.init(engineType);
		environment.loop();

		Logger::log(GENERAL, INFO, "Exiting program.");
	}
	catch (const CustomException & e)
	{
		Logger::log(e.getSource(), FATAL, std::string(e.what()));
		return 1;
	}
	Profiler::print();
	return EXIT_SUCCESS;
}
