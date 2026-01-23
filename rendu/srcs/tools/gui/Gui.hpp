#pragma once

class GLFWwindow;

class Gui
{
	public:
		Gui() = delete;
		~Gui() = delete;

		static void	generateGui(GLFWwindow * window);
};
