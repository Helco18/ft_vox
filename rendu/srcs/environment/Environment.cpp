#include "Environment.hpp"
#include "OBJModel.hpp"
#include "InputManager.hpp"

void Environment::init(EngineType engineType)
{
	if (!OBJModel::loadModels())
		throw std::runtime_error("Failed to load models.");

	WindowManager windowManager(engineType);
	windowManager.load();
}

void Environment::loop()
{
	while (_running)
	{
		_windowManager.loop();
		InputManager::interceptMouse(&_windowManager);
	}
}
