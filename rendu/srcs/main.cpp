#include "InputManager.hpp"
#include "OBJModel.hpp"
#include "Environment.hpp"
#include "CustomExceptions.hpp"
#include "Profiler.hpp"
#include <iostream>

int main(int ac, char ** av)
{
	if ((ac == 2 && std::string(av[1]) == "-p") || (ac == 3 && std::string(av[2]) == "-p"))
		Profiler::enable();
	EngineType engineType = (ac >= 2 && std::string(av[1]) == "gl") ? OPENGL : VULKAN;
	Environment environment;

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
