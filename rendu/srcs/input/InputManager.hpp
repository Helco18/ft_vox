#pragma once

#include "VulkanEngine.hpp"
#include "GLFW/glfw3.h"

class InputManager
{
	public:
		static void interceptInputs(GLFWwindow * window, int key, int, int action, int);
		static void	interceptMovements(GLFWwindow * window, Camera * camera, float deltaTime);
		static void	interceptMouse(GLFWwindow * window, Camera * camera);
};