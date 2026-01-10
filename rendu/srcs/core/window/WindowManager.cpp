#include "Environment.hpp"
#include "OpenGLEngine.hpp"
#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"
#include "InputManager.hpp"
#include "Logger.hpp"
#include "PipelineManager.hpp"
#include <iomanip>
#include <iostream>

WindowManager::WindowManager(EngineType engineType, Environment * environment):
	_environment(environment), _engineType(engineType), _width(WIDTH), _height(HEIGHT),
	_windowPosX(0), _windowPosY(0), _isFullscreen(false), _isSwapRequested(false)
{
	_camera = new Camera(glm::vec3(2.0f, 0.0f, 0.0f), _width, _height);
	_lastFpsUpdate = glfwGetTime();
}

WindowManager::~WindowManager()
{
	delete _engine;
	delete _camera;
	if (_window)
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}
	OBJModel::deleteModels();
}

void WindowManager::load()
{
	_window = _createWindow();

	if (_windowPosX && _windowPosY)
		glfwSetWindowPos(_window, _windowPosX, _windowPosY);

	switch (static_cast<int>(_engineType))
	{
		case VULKAN: _engine = new VulkanEngine(_window, _camera); break;
		case OPENGL: _engine = new OpenGLEngine(_window, _camera); break;
		default: throw WindowException("Unknown Engine type.");
	}

	_engine->load();
	PipelineManager::init(_engine);

	glfwSetWindowUserPointer(_window, this);
	glfwSetFramebufferSizeCallback(_window, WindowManager::framebufferResizeCallback);
	glfwSetKeyCallback(_window, InputManager::interceptInputs);
	glfwSetScrollCallback(_window, InputManager::interceptScroll);

	if (_isFullscreen)
	{
		GLFWmonitor * monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode * mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}		
}

bool WindowManager::drawFrame()
{
	static double currentTime = 0;
	static int frames = 0;
	std::ostringstream oss;
	double fps;

	if (glfwWindowShouldClose(_window))
	{
		_environment->stop();
		return false;
	}

	glfwPollEvents();

	if (_isSwapRequested)
	{
		swap();
		return false;
	}

	frames++;
	currentTime = glfwGetTime();
	if (currentTime - _lastFpsUpdate >= 1.0)
	{
		fps = frames / (currentTime - _lastFpsUpdate); // average FPS in last second
		oss << std::fixed << std::setprecision(2) << fps;
		glfwSetWindowTitle(_window, (std::string(_engineType == VULKAN ? "[Vulkan] " : "[OpenGL] ") + oss.str()).c_str());

		frames = 0;
		_lastFpsUpdate = currentTime;
	}

	return true;
}

void WindowManager::swap()
{
	glfwGetWindowPos(_window, &_windowPosX, &_windowPosY);

	delete _engine;
	if (_window)
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	_engineType = _engineType == VULKAN ? OPENGL : VULKAN;
	Logger::log(WINDOW, INFO, "Swapping to: " + std::string(ENGINE_NAME(_engineType)) + ".");
	load();

	_lastFpsUpdate = glfwGetTime();
	glfwFocusWindow(_window);
	_isSwapRequested = false;
	Logger::log(WINDOW, INFO, "Successfully swapped to: " + std::string(ENGINE_NAME(_engineType)) + ".");
}

void WindowManager::framebufferResizeCallback(GLFWwindow * window, int width, int height)
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));
	AEngine * engine = windowManager->getEngine();
	Camera * camera = engine->getCamera();
	camera->setWidth(width);
	camera->setHeight(height);
	windowManager->setWidth(width);
	windowManager->setHeight(height);
	engine->setFramebufferResized(true);
}

void WindowManager::toggleFullscreen()
{
	static int windowWidth = 0;
	static int windowHeight = 0;

	if (!_isFullscreen)
	{
		glfwGetWindowPos(_window, &_windowPosX, &_windowPosY);
		glfwGetWindowSize(_window, &windowWidth, &windowHeight);

		GLFWmonitor * monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode * mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		_camera->setWidth(mode->width);
		_camera->setHeight(mode->height);
		_width = mode->width;
		_height = mode->height;
		_isFullscreen = true;
	}
	else
	{
		glfwSetWindowMonitor(_window, nullptr, _windowPosX, _windowPosY, windowWidth, windowHeight, 0);
		_camera->setWidth(windowWidth);
		_camera->setHeight(windowHeight);
		_width = windowWidth;
		_height = windowHeight;
		_isFullscreen = false;
	}

	glfwFocusWindow(_window);
}
