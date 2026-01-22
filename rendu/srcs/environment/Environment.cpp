#include "Environment.hpp"
#include "CustomExceptions.hpp"
#include "OBJModel.hpp"
#include "InputManager.hpp"
#include "Profiler.hpp"
#include "WorldManager.hpp"
#include "BlockData.hpp"
#include "Logger.hpp"
#include "TextureAtlas.hpp"
#include "PipelineManager.hpp"

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

	Camera * camera = _windowManager->getCamera();
	camera->addPipelineToRender(PIPELINE_VOXEL);
	camera->addPipelineToRender(PIPELINE_WIREFRAME);

	BlockData::init();

	WorldManager::createWorld(WORLD_NAME);
	WorldManager::loadWorld(WORLD_NAME);

	Logger::log(ENVIRONMENT, INFO, "Environment started.");
}

void Environment::loop()
{
	double frameStart;
	double deltaTime;
	AEngine * engine;
	World * world;

	engine = _windowManager->getEngine();
	world = WorldManager::getWorld(WORLD_NAME);
	while (_running)
	{
		Profiler p("Environment::loop-while(_running)");
		frameStart = glfwGetTime();
		engine->beginFrame();
		InputManager::interceptMouse(_windowManager);
		InputManager::interceptMovements(_windowManager);
		if (!_windowManager->drawFrame())
		{
			engine = _windowManager->getEngine();
			if (!glfwWindowShouldClose(_windowManager->getWindow()) && world)
				world->reloadChunks(engine);
			continue;
		}
		if (world)
		{
			world->generateProcedurally(_windowManager->getCamera());
			world->render(engine, _windowManager->isWireframe() ? PIPELINE_WIREFRAME : PIPELINE_VOXEL);
		}
		_windowManager->getCamera()->renderViewMatrix(engine, engine->getEngineType());
		engine->endFrame();
		deltaTime = glfwGetTime() - frameStart;
		_windowManager->setDeltaTime(deltaTime);
	}
	_windowManager->destroy();
}
