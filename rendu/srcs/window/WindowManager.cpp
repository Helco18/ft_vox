#include "WindowManager.hpp"

WindowManager::WindowManager(EngineType engineType): _engineType(engineType)
{
	_camera = new Camera(glm::vec3(2.0f, 0.0f, 0.0f), WIDTH, HEIGHT);
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
	glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
	glfwSetKeyCallback(_window, InputManager::interceptInputs);
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
		InputManager::interceptMovements(_window, _engine->getCamera(), glfwGetTime() - timeStart);
		InputManager::interceptMouse(_window, _engine->getCamera());
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
}
