#pragma once

#include "Player.hpp"
#include "WindowManager.hpp"
#include "WorldManager.hpp"

#define SCROLL_SPEED 5.0f
#define SPEED_MULTIPLICATOR 20.0f
#define MAX_SPEED 4242.0f

class InputManager
{
	public:
		InputManager() = delete;
		~InputManager() = delete;

		static void interceptInputs(GLFWwindow * window, int key, int, int action, int);
		static void	interceptMovements(WindowManager * windowManager);
		static void	interceptMouse(WindowManager * windowManager);
		static void	interceptOneTimeClicks(GLFWwindow * window, int action, int key, int);
		static void	interceptScroll(GLFWwindow * window, double xoffset, double yoffset);
		static void	interceptFocus(GLFWwindow* window, int focused);
};
