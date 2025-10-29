#pragma once

#include "AEngine.hpp"
#include "OpenGLEngine.hpp"
#include "VulkanEngine.hpp"

class WindowManager
{
	public:
		WindowManager(EngineType engineType);
		~WindowManager();

		void			load();
		void			loop();
		void			swap();

		AEngine *		getEngine() const { return _engine; }
		GLFWwindow *	getWindow() const { return _window; }
		Camera *		getCamera() const { return _camera; }
		int				getWidth() const { return _width; }
		int				getHeight() const { return _height; }

		void			setWidth(int width) { _width = width; }
		void			setHeight(int height) { _height = height; }

		void			toggleFullscreen();

		static void		framebufferResizeCallback(GLFWwindow * window, int width, int height);

	private:
		EngineType		_engineType;
		AEngine *		_engine;
		GLFWwindow *	_window;
		Camera *		_camera;
		double			_lastFpsUpdate;
		int				_width;
		int				_height;
		bool			_isFullscreen;

		GLFWwindow *	_createWindow();
		void			_setIcon(GLFWwindow * window);
		void			_glfwErrorCallback(int error, const char * description);
		GLFWimage		_decodeOneStep(const char * filename);
};
