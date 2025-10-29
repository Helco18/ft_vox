#include <iostream>
#include "WindowManager.hpp"

int main(int ac, char ** av)
{
	EngineType engineType = ac == 2 && std::string(av[1]) == "gl" ? OPENGL : VULKAN;

	try
	{
		WindowManager windowManager(engineType);

		windowManager.load();
		windowManager.loop();

		std::cout << GREEN << "[OK] Exiting program." << RESET << std::endl;
	}
	catch (const std::exception & e)
	{
		std::cerr << RED << "[ERROR MAIN] " << e.what() << RESET << std::endl;
		return 3;
	}
	return EXIT_SUCCESS;
}
