#include "VulkanEngine.hpp"
#include "InputManager.hpp"
#include <iostream>

int main(void)
{
	try
	{
		Model::loadModels();

		GLFWwindow * window = getWindow();
		Camera camera(glm::vec3(2.0f));
		VulkanEngine engine(window, &camera);

		glfwSetWindowUserPointer(window, &engine);
		glfwSetFramebufferSizeCallback(window, engine.framebufferResizeCallback);
		glfwSetWindowUserPointer(window, &camera);
		glfwSetKeyCallback(window, InputManager::interceptInputs);
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
				glfwSetWindowTitle(window, toString(fps).c_str());

				frames = 0;
				lastTime = currentTime;
			}
		}

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
