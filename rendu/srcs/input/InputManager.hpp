#pragma once

#include "WindowManager.hpp"
#include "utils.hpp"

class InputManager
{
	public:
		static void interceptInputs(GLFWwindow * window, int key, int, int action, int);
		static void	interceptMovements(WindowManager * windowManager, float deltaTime);
		static void	interceptMouse(WindowManager * windowManager);
};