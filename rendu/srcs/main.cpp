#include "vulkan/VulkanEngine.hpp"
#include <exception>
#include <iostream>

int main(void)
{
	try
	{
		Model::loadModels();

		GLFWwindow * window = getWindow();
		VulkanEngine engine(window);

		glfwSetWindowUserPointer(window, &engine);
		glfwSetFramebufferSizeCallback(window, engine.framebufferResizeCallback);
		std::cout << GREEN << "[OK] Vulkan engine initialized successfully." << RESET << std::endl;
		

		double lastTime = glfwGetTime();
		int frames = 0;

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			engine.drawFrame();

			frames++;

			double currentTime = glfwGetTime();
			if (currentTime - lastTime >= 1.0)
			{
				double fps = frames / (currentTime - lastTime); // average FPS in last second
				std::cout << MAGENTA << "FPS: " << fps << "\r" << RESET << std::flush;

				frames = 0;
				lastTime = currentTime;
			}
		}

		engine.getDevice().waitIdle();
		std::cout << GREEN << "[OK] Exiting program." << RESET << std::endl;

		glfwDestroyWindow(window);
		glfwTerminate();
	}
	catch (const std::exception & e)
	{
		std::cerr << RED << "[ERROR MAIN] " << e.what() << RESET << std::endl;
		return 3;
	}
	return EXIT_SUCCESS;
}
