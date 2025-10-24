#include "VulkanEngine.hpp"
#include "OBJModel.hpp"
#include "InputManager.hpp"
#include <iostream>

int main(void)
{
	try
	{
		OBJModel model(TEST);
		if (!model.load())
			throw std::runtime_error("Tu réussiras jamais l'exam06");
		GLFWwindow * window = getWindow();
		Camera camera(glm::vec3(2.0f, 0.0f, 0.0f), WIDTH, HEIGHT);
		VulkanEngine engine(window, &camera);

		glfwSetWindowUserPointer(window, &engine);
		glfwSetFramebufferSizeCallback(window, engine.framebufferResizeCallback);
		glfwSetKeyCallback(window, InputManager::interceptInputs);

		std::cout << GREEN << "[OK] Vulkan engine initialized successfully." << RESET << std::endl;
		
		double lastTime = glfwGetTime();
		double timeStart;
		int frames = 0;

		while (!glfwWindowShouldClose(window))
		{
			timeStart = glfwGetTime();
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
			InputManager::interceptMovements(window, &camera, glfwGetTime() - timeStart);
			InputManager::interceptMouse(window, &camera);
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
