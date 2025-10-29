#pragma once

#include "WindowManager.hpp"
#include "utils.hpp"

class InputManager
{
	public:
		static void interceptInputs(GLFWwindow * window, int key, int, int action, int);
		static void	interceptMovements(GLFWwindow * window, Camera * camera, float deltaTime);
		static void	interceptMouse(GLFWwindow * window, Camera * camera);
};