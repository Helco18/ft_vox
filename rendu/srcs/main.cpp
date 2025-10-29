#include "VulkanEngine.hpp"
#include "OpenGLEngine.hpp"
#include "OBJModel.hpp"
#include "AEngine.hpp"
#include "InputManager.hpp"
#include <iostream>

int main(int ac, char **av)
{
	EngineType engineType = ac == 2 && std::string(av[1]) == "gl" ? OPENGL : VULKAN;

	try
	{
		GLFWwindow * window = getWindow(engineType);
		Camera * camera = new Camera(glm::vec3(2.0f, 0.0f, 0.0f), WIDTH, HEIGHT);
		AEngine * engine;

		if (!OBJModel::loadModels())
			throw std::runtime_error("Failed to load models.");
	
		switch (static_cast<int>(engineType))
		{
			case VULKAN: engine = new VulkanEngine(window, camera); break;
			case OPENGL: engine = new OpenGLEngine(window, camera); break;
			default: throw std::runtime_error("Unknown Engine type.");
		}

		engine->load();

		glfwSetWindowUserPointer(window, engine);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		glfwSetKeyCallback(window, InputManager::interceptInputs);

		double lastTime = glfwGetTime();
		double timeStart;
		int frames = 0;

		while (!glfwWindowShouldClose(window))
		{
			timeStart = glfwGetTime();
			glfwPollEvents();
			engine->drawFrame();

			frames++;

			double currentTime = glfwGetTime();
			if (currentTime - lastTime >= 1.0)
			{
				double fps = frames / (currentTime - lastTime); // average FPS in last second
				glfwSetWindowTitle(window, std::string(std::string(engineType == VULKAN ? "[Vulkan] " : "[OpenGL] ") + toString(fps)).c_str());

				frames = 0;
				lastTime = currentTime;
			}
			InputManager::interceptMovements(window, camera, glfwGetTime() - timeStart);
			InputManager::interceptMouse(window, camera);
		}

		std::cout << GREEN << "[OK] Exiting program." << RESET << std::endl;

		delete engine;
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
