#include "World.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <algorithm>

World::~World()
{
	_isLoaded.store(false);
	_cv.notify_one();
	_chunkPool.stop();
}

void World::load()
{
	_noise.setFBM(3, 0.5, 2.0);
	_noiseCave.setFBM(1, 0.5, 2.0);
	_isLoaded.store(true);
	_chunkPool.start(ThreadPool::getAvailableThreads() - 2); // Minus 2 for Vulkan threads
	_chunkPool.submitTask([this]() { _generateChunks(); });
}

void World::unloadChunks(AEngine * engine)
{
	for (const std::pair<const glm::ivec3, std::unique_ptr<Chunk>> & chunks : _chunkMap)
	{
		const std::unique_ptr<Chunk> & chunk = chunks.second;
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

	// glm::vec3 dir = camera->computeForward(0);
	// rayCast(camPos, dir, 20.0f);

	if (lastVisitedChunk == currentChunk && oldRenderDistance == renderDistance)
		return;
	if (oldRenderDistance != renderDistance)
	{
		oldRenderDistance = renderDistance;
		_computeRenderDistance(renderDistance);
	}
	if (lastVisitedChunk != currentChunk)
	{
		lastVisitedChunk = currentChunk;
		if (!_isLocked.load())
			_checkForChunkDeletion(engine, camera);
	}
	_renderPoint = camPos;
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
			_nextVisibleChunks.clear();
		}
		ChunkVec::iterator it = std::remove_if(_visibleChunks.begin(), _visibleChunks.end(), [](const Chunk * a) { return !a; });
		_visibleChunks.erase(it, _visibleChunks.end());
		_readyToSwap.store(false);
	}
	int i = 0;
	const Plane * planes = camera->getPlanes();
	for (Chunk * chunk : _dirtyChunks)
	{
		if (!chunk)
			continue;
		ChunkState state = chunk->getState();
		if (state >= MESHED)
		{
			if (state == UPLOADED)
				chunk->unload(engine);
			chunk->generateMesh();
			chunk->uploadAsset(engine);
		}
	}
	_dirtyChunks.clear();
	if (!_isLocked.load())
	{
		_uploadedChunks.clear();
		for (Chunk * chunk : _visibleChunks)
		{
			if (!chunk)
				continue;
			ChunkState state = chunk->getState();
			if (state == MESHED && i < MAX_UPLOAD_PER_FRAME)
			{
				chunk->uploadAsset(engine);
				i++;
			}
			if (state == UPLOADED && _chunkIsFrustum(planes, chunk))
				_uploadedChunks.push_back(chunk);
		}
	}
	std::sort(_uploadedChunks.begin(), _uploadedChunks.end(), [this](const Chunk * a, const Chunk * b)
		{ return a->getDistance(_renderPoint) > b->getDistance(_renderPoint);});
	for (Chunk * chunk : _uploadedChunks)
		chunk->drawAsset(engine, pipelineType);
}
