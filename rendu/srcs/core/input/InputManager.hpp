#pragma once

#include "WindowManager.hpp"

#define SCROLL_SPEED 5.0f
#define SPEED_MULTIPLICATOR 5.0f

class InputManager
{
	public:
		InputManager() = delete;
		~InputManager() = delete;

		static void interceptInputs(GLFWwindow * window, int key, int, int action, int);
		static void	interceptMovements(WindowManager * windowManager);
		static void	interceptMouse(WindowManager * windowManager);
		static void	interceptScroll(GLFWwindow * window, double xoffset, double yoffset);
};
