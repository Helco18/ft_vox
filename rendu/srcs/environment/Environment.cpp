#include "Environment.hpp"
#include "CustomExceptions.hpp"
#include "InputManager.hpp"
#include "WorldManager.hpp"
#include "TextureAtlas.hpp"
#include "Skybox.hpp"
#include "BlockData.hpp"

Environment::~Environment()
{
	WorldManager::destroy();
	TextureAtlas::destroy();
	delete _windowManager;
}

void Environment::init(ProgramParams & programParams)
{
	if (!OBJModel::loadModels())
		throw EnvironmentException("Failed to load models.");

	TextureAtlas::pushTexture("resources/assets/textures/dirt.png");
	TextureAtlas::pushTexture("resources/assets/textures/dirt_tmp.png");
	TextureAtlas::pushTexture("resources/assets/textures/grass_block_top.png");
	TextureAtlas::pushTexture("resources/assets/textures/blue_stone.png");
	TextureAtlas::createAtlas();

	_windowManager = new WindowManager(programParams.engineType, this);
	_windowManager->load();

	Camera * camera = _windowManager->getCamera();
	_player.setCamera(camera);
	camera->addPipelineToRender(PIPELINE_VOXEL);
	camera->addPipelineToRender(PIPELINE_WIREFRAME);
	camera->addPipelineToRender(PIPELINE_LINES);
	camera->addPipelineToRender(PIPELINE_SKYBOX);
	camera->setFOV(programParams.fov);
	camera->setRenderDistance(programParams.renderDistance);
	camera->setIgnoreYMovement(programParams.ignoreY);

	BlockData::init();

	WorldManager::createWorld(WORLD_NAME);
	WorldManager::loadWorld(WORLD_NAME);

	Logger::log(ENVIRONMENT, INFO, "Environment started.");
}

void Environment::loop()
{
	double frameStart;
	AEngine * engine;
	World * world;
	Camera * camera;
	Skybox sky;

	engine = _windowManager->getEngine();
	world = WorldManager::getWorld(WORLD_NAME);
	camera = _player.getCamera();
	sky.generateMesh();
	sky.uploadAsset(engine);
	while (_running)
	{
		frameStart = glfwGetTime();
		engine->beginFrame();
		engine->beginImGui();
		InputManager::interceptMouse(_windowManager);
		InputManager::interceptMovements(_windowManager);
		if (!_windowManager->drawFrame())
			continue;
		else if (_windowManager->isSwapRequested())
		{
			_windowManager->swap();
			engine = _windowManager->getEngine();
			if (!glfwWindowShouldClose(_windowManager->getWindow()) && world)
			{
				world->unloadChunks(engine);
				sky.uploadAsset(engine);
			}
			continue;
		}
		if (world)
		{
			world->update(camera);
			world->render(engine, _windowManager->isWireframe() ? PIPELINE_WIREFRAME : PIPELINE_VOXEL);
		}
		camera->renderViewMatrix(engine);
		sky.drawAsset(engine, PIPELINE_SKYBOX);
		engine->endFrame();
		_windowManager->setDeltaTime(glfwGetTime() - frameStart);
	}
	_windowManager->destroy();
}
