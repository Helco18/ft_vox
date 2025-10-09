#include "vulkan/VulkanEngine.hpp"
#include "colors.hpp"
#include "utils.hpp"
#include <exception>
#include <iostream>

int main(void)
{
	try
	{
		GLFWwindow * window = getWindow();
		VulkanEngine engine(window);
		std::cout << GREEN << "[OK] Vulkan engine initialized successfully." << RESET << std::endl;

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			engine.drawFrame();
		}

		engine.waitIdle();
		std::cout << GREEN << "[OK] Exiting program." << RESET << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	catch (const std::exception & e)
	{
		std::cerr << RED << "[ERROR] " << e.what() << RESET << std::endl;
		return 3;
	}
	return EXIT_SUCCESS;
}
