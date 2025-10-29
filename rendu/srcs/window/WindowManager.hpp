#pragma once

#include "AEngine.hpp"
#include "OpenGLEngine.hpp"
#include "VulkanEngine.hpp"
#include "InputManager.hpp"

class WindowManager
{
	public:
		WindowManager(EngineType engineType);
		~WindowManager();

		void		load();
		void		loop();
		void		swap();

		AEngine *		getEngine() const { return _engine; }

	private:
		EngineType		_engineType;
		AEngine *		_engine;
		GLFWwindow *	_window;
		Camera *		_camera;
		double			_lastFpsUpdate;

		GLFWwindow *	_createWindow();
		void			_setIcon(GLFWwindow * window);
		void			_glfwErrorCallback(int error, const char* description);
		GLFWimage		_decodeOneStep(const char * filename);
};
