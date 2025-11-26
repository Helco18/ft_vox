#include "InputManager.hpp"
#include "OBJModel.hpp"
#include "colors.hpp"
#include "Environment.hpp"
#include <iostream>

int main(int ac, char ** av)
{
	EngineType engineType = ac == 2 && std::string(av[1]) == "gl" ? OPENGL : VULKAN;
	Environment environment;

	try
	{
		environment.init(engineType);
		environment.loop();

		std::cout << GREEN << "[OK] Exiting program." << RESET << std::endl;
	}
	catch (const std::exception & e)
	{
		std::cerr << RED << "[ERROR MAIN] " << e.what() << RESET << std::endl;
		return 3;
	}
	return EXIT_SUCCESS;
}
