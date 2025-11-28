#pragma once

#include "WindowManager.hpp"

class InputManager
{
	public:
		InputManager() = delete;
		~InputManager() = delete;

		static void interceptInputs(GLFWwindow * window, int key, int, int action, int);
		static void	interceptMovements(WindowManager * windowManager, float deltaTime);
		static void	interceptMouse(WindowManager * windowManager);
		static void	interceptScroll(GLFWwindow * window, double xoffset, double yoffset);
};