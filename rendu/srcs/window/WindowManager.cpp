#include "WindowManager.hpp"

WindowManager::~WindowManager()
{
	SAFE_DELETE(_engine);
	if (_window)
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}
}

void WindowManager::load()
{
	Camera * camera = new Camera(glm::vec3(2.0f, 0.0f, 0.0f), WIDTH, HEIGHT);

	_window = getWindow(_engineType);

	if (!OBJModel::loadModels())
		throw std::runtime_error("Failed to load models.");

	switch (static_cast<int>(_engineType))
	{
		case VULKAN: _engine = new VulkanEngine(_window, camera); break;
		case OPENGL: _engine = new OpenGLEngine(_window, camera); break;
		default: throw std::runtime_error("Unknown Engine type.");
	}
	_engine->load();

	glfwSetWindowUserPointer(_window, _engine);
	glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
	glfwSetKeyCallback(_window, InputManager::interceptInputs);
}

void WindowManager::loop()
{
	double lastTime = glfwGetTime();
	double timeStart;
	int frames = 0;

	while (!glfwWindowShouldClose(_window))
	{
		timeStart = glfwGetTime();
		glfwPollEvents();
		_engine->drawFrame();

		frames++;

		double currentTime = glfwGetTime();
		if (currentTime - lastTime >= 1.0)
		{
			double fps = frames / (currentTime - lastTime); // average FPS in last second
			glfwSetWindowTitle(_window, std::string(std::string(_engineType == VULKAN ? "[Vulkan] " : "[OpenGL] ") + toString(fps)).c_str());

			frames = 0;
			lastTime = currentTime;
		}
		InputManager::interceptMovements(_window, _engine->getCamera(), glfwGetTime() - timeStart);
		InputManager::interceptMouse(_window, _engine->getCamera());
	}
}
