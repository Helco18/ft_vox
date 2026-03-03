#include "OpenGLEngine.hpp"
#include "VulkanEngine.hpp"
#include "Environment.hpp"
#include "CustomExceptions.hpp"
#include "InputManager.hpp"
#include "Logger.hpp"
#include <iomanip>

WindowManager::WindowManager(EngineType engineType, Environment * environment, bool vsync):
	_environment(environment), _engineType(engineType), _width(WIDTH), _height(HEIGHT),
	_windowPosX(0), _windowPosY(0), _isFullscreen(false), _isSwapRequested(false), _vsync(vsync)
{
	_camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), _width, _height);
	_lastFpsUpdate = glfwGetTime();
}

void WindowManager::destroy()
{
	if (!_isActive)
		return;
	delete _engine;
	delete _camera;
	if (_window)
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}
	_isActive = false;
}

WindowManager::~WindowManager()
{
	if (_isActive)
		destroy();
}

void WindowManager::load()
{
	_window = _createWindow();

	if (_windowPosX && _windowPosY)
		glfwSetWindowPos(_window, _windowPosX, _windowPosY);

	switch (_engineType)
	{
		case VULKAN: _engine = new VulkanEngine(_window); break;
		case OPENGL: _engine = new OpenGLEngine(_window); break;
		default: throw WindowException("Unknown Engine type.");
	}

	glfwSetWindowUserPointer(_window, this);
	glfwSetFramebufferSizeCallback(_window, WindowManager::framebufferResizeCallback);
	glfwSetMouseButtonCallback(_window, InputManager::interceptOneTimeClicks);
	glfwSetKeyCallback(_window, InputManager::interceptInputs);
	glfwSetScrollCallback(_window, InputManager::interceptScroll);
	glfwSetInputMode(_window, GLFW_CURSOR, !_isMouseEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	glfwFocusWindow(_window);

	_engine->load();
	if (_vsync)
		_engine->setVsync(_vsync);
	PipelineManager::init(_engine);


	if (_isFullscreen)
	{
		GLFWmonitor * monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode * mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	_isActive = true;
}

bool WindowManager::drawFrame()
{
	static double currentTime = 0;
	static int frames = 0;

	if (glfwWindowShouldClose(_window))
	{
		_environment->stop();
		return false;
	}

	glfwPollEvents();

	frames++;
	currentTime = glfwGetTime();
	if (currentTime - _lastFpsUpdate >= 1.0)
	{
		double fps;
		std::ostringstream oss;
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
	Camera * camera = windowManager->getCamera();
	camera->setWidth(width);
	camera->setHeight(height);
	windowManager->setWidth(width);
	windowManager->setHeight(height);
	engine->setFramebufferResized(true);
	camera->renderViewMatrix(windowManager->getEngine());
}

void WindowManager::toggleMouse()
{
	_isMouseEnabled = !_isMouseEnabled;
	glfwSetInputMode(_window, GLFW_CURSOR, !_isMouseEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	glfwSetCursorPos(_window, (static_cast<float>(_width) / 2), (static_cast<float>(_height) / 2));
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
