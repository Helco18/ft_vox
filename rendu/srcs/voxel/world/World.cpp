#include "World.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <algorithm>

World::~World()
{
	_isLoaded.store(false);
	_cv.notify_one();
	_chunkPool.stop();
	for (std::pair<glm::ivec3, Chunk *> chunks : _chunkMap)
		delete chunks.second;
}

// geter

Chunk * World::getChunkAt(int x, int y, int z)
{
	int chunkX;
	int chunkY;
	int chunkZ;

	chunkX = static_cast<int>(std::floor(static_cast<double>(x) / CHUNK_WIDTH));
	chunkY = static_cast<int>(std::floor(static_cast<double>(y) / CHUNK_HEIGHT));
	chunkZ = static_cast<int>(std::floor(static_cast<double>(z) / CHUNK_LENGTH));
	return getChunkAtChunkLocation(chunkX, chunkY, chunkZ);
}

Chunk * World::getChunkAtChunkLocation(int x, int y, int z)
{
	std::lock_guard<std::mutex> lg(_mapMutex);
	ChunkMap::const_iterator it = _chunkMap.find(glm::ivec3(x, y, z));
	if (it != _chunkMap.end())
		return it->second;
	return nullptr;
}

inline Chunk * World::getChunkAt(const glm::vec3 & location)
{
	return getChunkAt(location.x, location.y, location.z);
}

inline Chunk * World::getChunkAtChunkLocation(const glm::vec3 & location)
{
	return getChunkAtChunkLocation(location.x, location.y, location.z);
}

// function public

void World::load()
{
	_noise.setFBM(3, 0.5, 2.0);
	_isLoaded.store(true);
	_chunkPool.start(ThreadPool::getAvailableThreads() - 2); // Minus 2 for Vulkan threads
	_chunkPool.submitTask([this]() { _generateChunks(); });
}

void World::unloadChunks(AEngine * engine)
{
	for (std::pair<glm::ivec3, Chunk *> chunks : _chunkMap)
	{
		Chunk * chunk = chunks.second;
		if (chunk && chunk->getState() == UPLOADED)
			chunk->unload(engine);
	}
}

void World::update(AEngine * engine, Camera * camera)
{
	static glm::ivec3 lastVisitedChunk(0, 0, 0);
	static int oldRenderDistance = 0;
	const glm::vec3 & camPos = camera->getPosition();
	const glm::ivec3 currentChunk = Chunk::posToChunkPos(camPos);
	const int renderDistance = camera->getRenderDistance();

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
		std::lock_guard<std::mutex> lg(_visibleChunksMutex);
		_visibleChunks = _nextVisibleChunks;
		std::sort(_visibleChunks.begin(), _visibleChunks.end(), [this](const Chunk * a, const Chunk * b)
				{ return a->getDistance(_renderPoint) > b->getDistance(_renderPoint);});
		_nextVisibleChunks.clear();
		_readyToSwap.store(false);
	}
	_extractPlanesFromProjmat(camera);
	int i = 0;
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
		else if (state == UPLOADED && _chunkIsFrustum(chunk))
			chunk->drawAsset(engine, pipelineType);
	}
}
