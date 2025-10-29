#include "WindowManager.hpp"
#include "InputManager.hpp"

WindowManager::WindowManager(EngineType engineType): _engineType(engineType), _width(WIDTH), _height(HEIGHT), _isFullscreen(false)
{
	_camera = new Camera(glm::vec3(2.0f, 0.0f, 0.0f), _width, _height);
}

WindowManager::~WindowManager()
{
	SAFE_DELETE(_engine);
	SAFE_DELETE(_camera);
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

	if (!OBJModel::loadModels())
		throw std::runtime_error("Failed to load models.");

	switch (static_cast<int>(_engineType))
	{
		case VULKAN: _engine = new VulkanEngine(_window, _camera); break;
		case OPENGL: _engine = new OpenGLEngine(_window, _camera); break;
		default: throw std::runtime_error("Unknown Engine type.");
	}

	_engine->load();

	glfwSetWindowUserPointer(_window, this);
	glfwSetFramebufferSizeCallback(_window, WindowManager::framebufferResizeCallback);
	glfwSetKeyCallback(_window, InputManager::interceptInputs);

	if (_isFullscreen && _engineType == OPENGL)
	{
		GLFWmonitor * monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode * mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
}

void WindowManager::loop()
{
	double timeStart;
	double currentTime;
	int frames = 0;

	_lastFpsUpdate = glfwGetTime();
	while (!glfwWindowShouldClose(_window))
	{
		timeStart = glfwGetTime();
		glfwPollEvents();
		_engine->drawFrame();

		frames++;

		currentTime = glfwGetTime();
		if (currentTime - _lastFpsUpdate >= 1.0)
		{
			double fps = frames / (currentTime - _lastFpsUpdate); // average FPS in last second
			glfwSetWindowTitle(_window, std::string(std::string(_engineType == VULKAN ? "[Vulkan] " : "[OpenGL] ") + toString(fps)).c_str());

			frames = 0;
			_lastFpsUpdate = currentTime;
		}
		InputManager::interceptMovements(this, glfwGetTime() - timeStart);
		InputManager::interceptMouse(this);
	}
}

void WindowManager::swap()
{
	SAFE_DELETE(_engine);
	if (_window)
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	_engineType = _engineType == VULKAN ? OPENGL : VULKAN;
	load();

	_lastFpsUpdate = glfwGetTime();
	glfwFocusWindow(_window);
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
	static int windowPosX = 0;
	static int windowPosY = 0;
	static int windowWidth = 0;
	static int windowHeight = 0;

	if (!_isFullscreen)
	{
		glfwGetWindowPos(_window, &windowPosX, &windowPosY);
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
		glfwSetWindowMonitor(_window, nullptr, windowPosX, windowPosY, windowWidth, windowHeight, 0);
		_camera->setWidth(windowWidth);
		_camera->setHeight(windowHeight);
		_width = windowWidth;
		_height = windowHeight;
		_isFullscreen = false;
	}

	glfwFocusWindow(_window);
}
