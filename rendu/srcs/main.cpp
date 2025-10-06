#include "vulkan/VulkanEngine.hpp"
#include "tools/colors.hpp"
#include <exception>
#include <iostream>

int main(void)
{

	try
	{	
		VulkanEngine	engine;

		engine.loop();
	}
	catch (const std::exception & e)
	{
		std::cerr << RED << "[ERROR] " << e.what() << RESET << std::endl;
		return 1;
	}
	return 0;
}
