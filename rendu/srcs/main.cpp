#include "vulkan/VulkanEngine.hpp"
#include "tools/colors.hpp"
#include <exception>
#include <iostream>

int main(void)
{
	if (glfwInit() == GLFW_FALSE)
	{
		std::cerr << BOLD_RED << "Failed to create GLFW context." << std::endl;
		return 1;
	}

	// Vu que GLFW créé un contexte OpenGL par défaut, on lui précise de ne pas le faire.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// On va gérer nous même le resize de la fenêtre
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Width, height, titre de la fenêtre, écran sur lequel afficher (GLFWMonitor, nullptr pour défaut)
	// Le dernier paramètre est pour OpenGL, on met donc nullptr.
	GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "scop", nullptr, nullptr);
	if (!window)
	{
		std::cerr << BOLD_RED << "Failed to instantiate GLFW window." << RESET << std::endl;
		return 2;
	}

	try
	{
		VulkanEngine	engine(window);

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}
	catch (const std::exception & e)
	{
		std::cerr << RED << "[ERROR] " << e.what() << RESET << std::endl;
		return 3;
	}
	return EXIT_SUCCESS;
}
