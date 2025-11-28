#pragma once

#include "AEngine.hpp"
#include "utils.hpp"

class Environment;

class WindowManager
{
	public:
		WindowManager(EngineType engineType, Environment * environment);
		~WindowManager();

		void			load();
		bool			drawFrame();
		void			swap();

		AEngine *		getEngine() const { return _engine; }
		GLFWwindow *	getWindow() const { return _window; }
		Camera *		getCamera() const { return _camera; }
		int				getWidth() const { return _width; }
		int				getHeight() const { return _height; }
		bool			isSwapRequested() const { return _isSwapRequested; }

		void			setWidth(int width) { _width = width; }
		void			setHeight(int height) { _height = height; }
		void			setWindowPosX(int windowPosX) { _windowPosX = windowPosX; }
		void			setWindowPosY(int windowPosY) { _windowPosY = windowPosY; }
		void			requestSwap() { _isSwapRequested = true; }

		void			toggleFullscreen();

		static void		framebufferResizeCallback(GLFWwindow * window, int width, int height);

	private:
		Environment *	_environment;
		EngineType		_engineType;
		AEngine *		_engine;
		GLFWwindow *	_window;
		Camera *		_camera;
		double			_lastFpsUpdate;
		int				_width;
		int				_height;
		int				_windowPosX;
		int				_windowPosY;
		bool			_isFullscreen;
		bool			_isSwapRequested;

		GLFWwindow *	_createWindow();
		void			_setIcon(GLFWwindow * window);
		void			_glfwErrorCallback(int error, const char * description);
		GLFWimage		_decodeOneStep(const char * filename);
};
