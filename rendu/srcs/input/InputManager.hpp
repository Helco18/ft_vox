#pragma once

#include "Camera.hpp"
#include "GLFW/glfw3.h"

class InputManager
{
	public:
		static void interceptInputs(GLFWwindow * window, int key, int scancode, int action, int mods);
};
