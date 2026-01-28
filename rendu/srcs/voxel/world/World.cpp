#include "World.hpp"
#include "Logger.hpp"
#include "Profiler.hpp"
#include <algorithm>

World::~World()
{
	_isLoaded.store(false);
	_cv.notify_one();
	_chunkPool.stop();
	for (std::pair<glm::ivec3, Chunk *> chunks : _chunkMap)
		delete chunks.second;
}

void World::load()
{
	_isLoaded.store(true);
	_chunkPool.start(ThreadPool::getAvailableThreads() - 2); // Minus 2 for Vulkan
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

Chunk * World::getChunkAt(int x, int y, int z)
{
	int chunkX;
	int chunkY;
	int chunkZ;

	Profiler p("getChunkAt");
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

static int getRenderDistanceMin()
{
	if (CHUNK_HEIGHT <= CHUNK_WIDTH && CHUNK_HEIGHT <= CHUNK_LENGTH)
		return CHUNK_HEIGHT;
	if (CHUNK_WIDTH <= CHUNK_HEIGHT && CHUNK_WIDTH <= CHUNK_LENGTH)
		return CHUNK_WIDTH;
	if (CHUNK_LENGTH <= CHUNK_HEIGHT && CHUNK_LENGTH <= CHUNK_WIDTH)
		return CHUNK_WIDTH;
	return CHUNK_LENGTH;
}

void World::_computeRenderDistance(const int renderDistance)
{
	int renderDistanceMin = getRenderDistanceMin();

	int ratioW = CHUNK_WIDTH / renderDistanceMin;
	int ratioH = CHUNK_HEIGHT / renderDistanceMin;
	int ratioL = CHUNK_LENGTH / renderDistanceMin;

	_renderDistanceX = (renderDistance / ratioW) == 0 ? 1 : (renderDistance / ratioW);
	_renderDistanceY = (renderDistance / ratioH) == 0 ? 1 : renderDistance / ratioH;
	_renderDistanceZ = (renderDistance / ratioL) == 0 ? 1 : (renderDistance / ratioL);
}

bool World::_isWithinRenderDistance(const glm::vec3 & chunkPos, const glm::vec3 & camPos)
{
	return !(chunkPos.x > static_cast<int>(std::floor(camPos.x / CHUNK_WIDTH)) + _renderDistanceX
		|| chunkPos.x < static_cast<int>(std::floor(camPos.x / CHUNK_WIDTH)) - _renderDistanceX
		|| chunkPos.y > static_cast<int>(std::floor(camPos.y / CHUNK_HEIGHT)) + _renderDistanceY
		|| chunkPos.y < static_cast<int>(std::floor(camPos.y / CHUNK_HEIGHT)) - _renderDistanceY
		|| chunkPos.z > static_cast<int>(std::floor(camPos.z / CHUNK_LENGTH)) + _renderDistanceZ
		|| chunkPos.z < static_cast<int>(std::floor(camPos.z / CHUNK_LENGTH)) - _renderDistanceZ);
}

World::ChunkVec World::_queryChunksInRange(ChunkState minState)
{
	ChunkVec chunks;
	glm::vec3 cameraPosition = _renderPoint;
	cameraPosition.x /= CHUNK_WIDTH;
	cameraPosition.y /= CHUNK_HEIGHT;
	cameraPosition.z /= CHUNK_LENGTH;

	int renderDistanceNorth = _renderDistanceX + cameraPosition.x;
	int renderDistanceSouth = cameraPosition.x - _renderDistanceX;
	int renderDistanceEast = _renderDistanceZ + cameraPosition.z;
	int renderDistanceWest = cameraPosition.z - _renderDistanceZ;
	int renderDistanceUp = _renderDistanceY + cameraPosition.y;
	int renderDistanceDown = cameraPosition.y - _renderDistanceY;

	for (int x = renderDistanceSouth; x < renderDistanceNorth; ++x)
	{
		for (int y = renderDistanceDown; y < renderDistanceUp; ++y)
		{
			for (int z = renderDistanceWest; z < renderDistanceEast; ++z)
			{
				glm::vec3 location(x, y, z);
				std::lock_guard<std::mutex> lg(_mapMutex);
				ChunkMap::iterator it = _chunkMap.find(location);
				if (it != _chunkMap.end())
				{
					Chunk * chunk = it->second;
					if (chunk->getState() >= minState)
						chunks.push_back(it->second);
					continue;
				}
				Chunk * chunk = new Chunk(x, y, z, this);
				_chunkMap.try_emplace(location, chunk);
				if (chunk->getState() >= minState)
					chunks.push_back(chunk);
			}
		}
	}

	std::lock_guard<std::mutex> lg(_renderPointMutex);
	std::sort(chunks.begin(), chunks.end(), [this](const Chunk * a, const Chunk * b)
		{ return a->getDistance(_renderPoint) < b->getDistance(_renderPoint);});
	return chunks;
}

void World::_generateChunks()
{
	ChunkVec chunksToGenerate;
	std::mutex cvMutex;

	while (_isLoaded.load())
	{
		std::unique_lock<std::mutex> lock(cvMutex);
		_cv.wait(lock, [&] { return !_isLoaded.load() || _isProceduralRequested.load(); });
		if (!_isLoaded.load())
			return;
		_isProceduralRequested.store(false);
		ChunkVec newChunks = _queryChunksInRange();
		if (newChunks.empty())
			continue;
		for (Chunk * chunk : newChunks)
		{
			if (_isProceduralRequested.load())
				break;
			if (chunk->getState() == NONE)
			{
				chunk->setState(BUILDING);
				_chunkPool.submitTask([chunk]() { chunk->build(); chunk->generateMesh(); });
			}
		}
	}
}

void World::update(Camera * camera)
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
		lastVisitedChunk = currentChunk;
	{
		std::lock_guard<std::mutex> lg(_renderPointMutex);
		_renderPoint = camPos;
	}
	_visibleChunks = _queryChunksInRange();
	std::sort(_visibleChunks.begin(), _visibleChunks.end(), [this](const Chunk * a, const Chunk * b)
			{ return a->getDistance(_renderPoint) > b->getDistance(_renderPoint);});
	_isProceduralRequested.store(true);
	_cv.notify_one();
}

void World::render(AEngine * engine, PipelineType pipelineType)
{
	for (Chunk * chunk : _visibleChunks)
	{
		ChunkState state = chunk->getState();
		if (state == MESHED)
			chunk->uploadAsset(engine);
		else if (state == UPLOADED)
			chunk->drawAsset(engine, pipelineType);
	}
}
