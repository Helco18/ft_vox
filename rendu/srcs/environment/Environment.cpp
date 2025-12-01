#include "Environment.hpp"
#include "OBJModel.hpp"
#include "InputManager.hpp"
#include "WorldManager.hpp"
#include "colors.hpp"
#include <iostream>

Environment::~Environment()
{
	WorldManager::destroy();
	delete _windowManager;
}

void Environment::init(EngineType engineType)
{
	if (!OBJModel::loadModels())
		throw std::runtime_error("Failed to load models.");

	_windowManager = new WindowManager(engineType, this);
	_windowManager->load();

	WorldManager::createWorld("bozoandzibocircus");

	std::cout << GREEN << "[OK] Environment started" << RESET << std::endl;
}

void Environment::loop()
{
	double frameStart;

	while (_running)
	{
		frameStart = glfwGetTime();
		_windowManager->getEngine()->beginFrame();
		InputManager::interceptMouse(_windowManager);
		InputManager::interceptMovements(_windowManager, glfwGetTime() - frameStart);
		World * world = WorldManager::getWorld("bozoandzibocircus");
		if (world)
			world->render(_windowManager->getEngine());
		if (!_windowManager->drawFrame())
		{
			if (!glfwWindowShouldClose(_windowManager->getWindow()) && world)
				world->reloadChunks();
			continue;
		}
		_windowManager->getEngine()->endFrame();
	}
}
