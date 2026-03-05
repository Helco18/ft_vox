#include "Environment.hpp"
#include "CustomExceptions.hpp"
#include "InputManager.hpp"
#include "WorldManager.hpp"
#include "TextureAtlas.hpp"
#include "Skybox.hpp"
#include "BlockData.hpp"
#include "Crosshair.hpp"
#include "BlockOverlay.hpp"
#include "BiomeManager.hpp"

Environment::~Environment()
{
	WorldManager::destroy();
	TextureAtlas::destroy();
	delete _windowManager;
}

void Environment::init(ProgramParams & programParams)
{
	TextureAtlas::pushFolder("resources/assets/textures");
	TextureAtlas::createAtlas();

	_windowManager = new WindowManager(programParams.engineType, this, programParams.vsync);
	_windowManager->load();

	Camera * camera = _windowManager->getCamera();
	camera->setPosition(programParams.spawnLocation);
	_player.setCamera(camera);
	camera->addPipelineToRender(PIPELINE_VOXEL);
	camera->addPipelineToRender(PIPELINE_WIREFRAME);
	camera->addPipelineToRender(PIPELINE_LINES);
	camera->addPipelineToRender(PIPELINE_SKYBOX);
	camera->addPipelineToRender(PIPELINE_BLOCKOVERLAY);
	camera->setFOV(programParams.fov);
	camera->setRenderDistance(programParams.renderDistance);

	BlockData::init();
	BiomeManager::init(programParams.seed);

	WorldManager::createWorld(WORLD_NAME, programParams.seed);
	WorldManager::loadWorld(WORLD_NAME);
	_player.setWorld(WorldManager::getWorld(WORLD_NAME));

	Logger::log(ENVIRONMENT, INFO, "Environment started.");
}

void Environment::loop()
{
	double frameStart;
	AEngine * engine;
	World * world;
	Camera * camera;
	Skybox sky;
	Crosshair crosshair;
	BlockOverlay blockOverlay;
	TargetedBlock targetedBlock;

	engine = _windowManager->getEngine();
	world = WorldManager::getWorld(WORLD_NAME);
	camera = _player.getCamera();
	sky.generateMesh();
	sky.uploadAsset(engine);
	crosshair.generateMesh();
	crosshair.uploadAsset(engine);
	blockOverlay.generateMesh();
	blockOverlay.uploadAsset(engine);
	while (_running)
	{
		frameStart = glfwGetTime();
		if (!_windowManager->drawFrame())
			continue;
		else if (_windowManager->isSwapRequested())
		{
			if (world)
				world->unloadChunks(engine);
			sky.unload(engine);
			crosshair.unload(engine);
			blockOverlay.unload(engine);
			_windowManager->swap();
			engine = _windowManager->getEngine();
			sky.uploadAsset(engine);
			crosshair.uploadAsset(engine);
			blockOverlay.uploadAsset(engine);
			continue;
		}
		targetedBlock = _player.getTargetedBlock();
		engine->beginFrame();
		engine->beginImGui();
		InputManager::interceptMouse(_windowManager);
		InputManager::interceptMovements(_windowManager);
		crosshair.drawAsset(engine, _windowManager->getWidth(), _windowManager->getHeight());
		blockOverlay.drawAsset(engine, targetedBlock);
		if (world)
		{
			world->update(engine, camera);
			world->render(engine, _windowManager->isWireframe() ? PIPELINE_WIREFRAME : PIPELINE_VOXEL, camera);
		}
		camera->renderViewMatrix(engine);
		sky.drawAsset(engine);
		engine->endFrame();
		_windowManager->setDeltaTime(glfwGetTime() - frameStart);
	}
	_windowManager->destroy();
}
