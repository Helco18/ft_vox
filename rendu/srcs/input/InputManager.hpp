#pragma once

#include "WindowManager.hpp"

class InputManager
{
	public:
		static void interceptInputs(GLFWwindow * window, int key, int, int action, int);
		static void	interceptMovements(WindowManager * windowManager, float deltaTime);
		static void	interceptMouse(WindowManager * windowManager);
};