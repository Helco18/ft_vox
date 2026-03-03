#include "World.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <algorithm>

World::~World()
{
	_isLoaded.store(false);
	_cv.notify_one();
	_generatePool.stop();
	_meshPool.stop();
}

void World::load()
{
	_terrainNoise.setFBM(3, 0.5, 2.0);
	_temperatureNoise.setFBM(2, 0.5, 2.0);
	_caveNoise.setFBM(1, 0.5, 2.0);
	_heightNoise.setFBM(3, 1.0, 2.0);
	_netherNoise.setFBM(2, 0.5, 2.0);
	_isLoaded.store(true);
	_generatePool.start(ThreadPool::getAvailableThreads() / 2);
	_meshPool.start(ThreadPool::getAvailableThreads());
	_generatePool.submitTask([this]() { _generateChunks(); });
}

void World::unloadChunks(AEngine * engine)
{
	for (const std::pair<const glm::ivec3, std::shared_ptr<Chunk>> & chunks : _chunkMap)
	{
		std::shared_ptr<Chunk> chunk = chunks.second;
		if (chunk && chunk->getState() == UPLOADED)
			chunk->unload(engine);
	}
}

void World::update(AEngine * engine, Camera * camera)
{
	static glm::ivec3 lastVisitedChunk(0, 0, 0);
	static int oldRenderDistance = 0;
	const glm::vec3 & camPos = camera->getPosition();
	const glm::ivec3 currentChunk = Chunk::locToChunkLoc(camPos);
	const int renderDistance = camera->getRenderDistance();

	if (lastVisitedChunk == currentChunk && oldRenderDistance == renderDistance)
		return;
	if (oldRenderDistance != renderDistance)
	{
		oldRenderDistance = renderDistance;
		_computeRenderDistance(renderDistance);
	}
	_renderPoint = camPos;
	if (lastVisitedChunk != currentChunk)
	{
		lastVisitedChunk = currentChunk;
		if (!_isLocked.load())
		{
			_generatePool.clearTasks();
			_checkForChunkDeletion(engine, camera);
		}
	}
	_isProceduralRequested.store(true);
	_cv.notify_one();
}

void World::render(AEngine * engine, PipelineType pipelineType, Camera * camera)
{
	if (_readyToSwap)
	{
		{
			std::lock_guard<std::mutex> lg(_visibleChunksMutex);
			_visibleChunks = _nextVisibleChunks;
			_nextVisibleChunks = {};
		}
		_readyToSwap.store(false);
	}
	for (const std::pair<std::weak_ptr<Chunk>, glm::vec3> & chunkPair : _dirtyChunks)
	{
		std::shared_ptr<Chunk> chunk = chunkPair.first.lock();
		if (!chunk)
			continue;
		const glm::vec3 & pos = chunkPair.second;
		ChunkState state = chunk->getState();
		if (state >= MESHED)
		{
			if (state == UPLOADED)
				chunk->unloadMesh(engine);
			chunk->updateMesh(pos);
			if (chunk->getState() != MESHED_EMPTY)
				chunk->uploadAsset(engine);
		}
	}
	_dirtyChunks = {};
	int i = 0;
	const Plane * planes = camera->getPlanes();
	if (!_isLocked.load())
	{
		_uploadedChunks = {};
		for (std::shared_ptr<Chunk> chunk : _visibleChunks)
		{
			if (!chunk)
				continue;
			ChunkState state = chunk->getState();
			if (state == MESHED && i < MAX_UPLOAD_PER_FRAME)
			{
				chunk->uploadAsset(engine);
				i++;
			}
			if (state == UPLOADED && _chunkIsFrustum(planes, camera->getPosition(), chunk.get()))
				_uploadedChunks.push_back(chunk);
		}
	}
	std::sort(_uploadedChunks.begin(), _uploadedChunks.end(), [this](const std::shared_ptr<Chunk> a, const std::shared_ptr<Chunk> b)
		{ return a->getDistance(_renderPoint) > b->getDistance(_renderPoint);});
	for (std::shared_ptr<Chunk> chunk : _uploadedChunks)
		chunk->drawAsset(engine, pipelineType);
}
