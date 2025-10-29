#pragma once

#include "AEngine.hpp"
#include "OpenGLEngine.hpp"
#include "VulkanEngine.hpp"
#include "InputManager.hpp"

class WindowManager
{
	public:
		WindowManager(EngineType engineType) : _engineType(engineType) {}
		~WindowManager();

        void    load();
        void    loop();

	private:
		AEngine *		_engine;
		GLFWwindow *	_window;
        EngineType      _engineType;
};
