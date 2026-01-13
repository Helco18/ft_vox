#include "Environment.hpp"
#include "CustomExceptions.hpp"
#include "OBJModel.hpp"
#include "InputManager.hpp"
#include "WorldManager.hpp"
#include "BlockData.hpp"
#include "Logger.hpp"
#include "TextureAtlas.hpp"
#include "PipelineManager.hpp"
#include <iostream>

Environment::~Environment()
{
	WorldManager::destroy();
	TextureAtlas::destroy();
	delete _windowManager;
}

void Environment::init(EngineType engineType)
{
	if (!OBJModel::loadModels())
		throw EnvironmentException("Failed to load models.");

	TextureAtlas::pushTexture("assets/textures/stone.png");
	TextureAtlas::pushTexture("assets/textures/blue_stone.png");
	TextureAtlas::createAtlas();

	_windowManager = new WindowManager(engineType, this);
	_windowManager->load();

	BlockData::init();

	WorldManager::createWorld(WORLD_NAME);

	Logger::log(ENVIRONMENT, INFO, "Environment started.");
}

void Environment::loop()
{
	double frameStart;
	double deltaTime = 0.0;

	while (_running)
	{
		frameStart = glfwGetTime();
		_windowManager->getEngine()->beginFrame();
		InputManager::interceptMouse(_windowManager);
		InputManager::interceptMovements(_windowManager, deltaTime);
		World * world = WorldManager::getWorld(WORLD_NAME);
		if (world)
		{
			world->generateProcedurally(_windowManager->getCamera());
			world->render(_windowManager->getEngine(), _windowManager->isWireframe() ? PIPELINE_WIREFRAME : PIPELINE_VOXEL);
		}
		if (!_windowManager->drawFrame())
		{
			if (!glfwWindowShouldClose(_windowManager->getWindow()) && world)
				world->reloadChunks(_windowManager->getEngine());
			continue;
		}
		_windowManager->getEngine()->endFrame();
		deltaTime = glfwGetTime() - frameStart;
	}
	_windowManager->destroy();
}
