#include "World.hpp"

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
	int borderedRenderDistance = renderDistance + RENDER_DISTANCE_BORDER;
	int renderDistanceMin = getRenderDistanceMin();

	int ratioW = CHUNK_WIDTH / renderDistanceMin;
	int ratioH = CHUNK_HEIGHT / renderDistanceMin;
	int ratioL = CHUNK_LENGTH / renderDistanceMin;

	// Added +1 for 'borders' that only get meshed after their neighbor gets built
	_renderDistance.x = (borderedRenderDistance / ratioW) == 0 ? 1 : (borderedRenderDistance / ratioW);
	_renderDistance.y = (borderedRenderDistance / ratioH) == 0 ? 1 : (borderedRenderDistance / ratioH);
	_renderDistance.z = (borderedRenderDistance / ratioL) == 0 ? 1 : (borderedRenderDistance / ratioL);
}

bool World::_isWithinRenderDistance(const glm::vec3 & chunkPos, const glm::vec3 & camPos)
{
	return !(chunkPos.x > static_cast<int>(std::floor(camPos.x / CHUNK_WIDTH)) + _renderDistance.x
		|| chunkPos.x < static_cast<int>(std::floor(camPos.x / CHUNK_WIDTH)) - _renderDistance.x
		|| chunkPos.y > static_cast<int>(std::floor(camPos.y / CHUNK_HEIGHT)) + _renderDistance.y
		|| chunkPos.y < static_cast<int>(std::floor(camPos.y / CHUNK_HEIGHT)) - _renderDistance.y
		|| chunkPos.z > static_cast<int>(std::floor(camPos.z / CHUNK_LENGTH)) + _renderDistance.z
		|| chunkPos.z < static_cast<int>(std::floor(camPos.z / CHUNK_LENGTH)) - _renderDistance.z);
}

World::ChunkVec World::_queryChunksInRange()
{
	ChunkVec chunks;
	glm::vec3 cameraPosition = _renderPoint;
	cameraPosition.x /= CHUNK_WIDTH;
	cameraPosition.y /= CHUNK_HEIGHT;
	cameraPosition.z /= CHUNK_LENGTH;

	int renderDistanceNorth = _renderDistance.x + cameraPosition.x;
	int renderDistanceSouth = cameraPosition.x - _renderDistance.x;
	int renderDistanceEast = _renderDistance.z + cameraPosition.z;
	int renderDistanceWest = cameraPosition.z - _renderDistance.z;
	int renderDistanceUp = _renderDistance.y + cameraPosition.y;
	int renderDistanceDown = cameraPosition.y - _renderDistance.y;

	for (int x = renderDistanceSouth; x < renderDistanceNorth; ++x)
	{
		for (int y = renderDistanceDown; y < renderDistanceUp; ++y)
		{
			for (int z = renderDistanceWest; z < renderDistanceEast; ++z)
			{
				glm::ivec3 location(x, y, z);
				std::lock_guard<std::mutex> lg(_mapMutex);
				ChunkMap::iterator it = _chunkMap.find(location);
				if (it != _chunkMap.end())
				{
					chunks.push_back(it->second);
					continue;
				}
				Chunk * chunk = new Chunk(x, y, z, this);
					_chunkMap.try_emplace(location, chunk);
				chunks.push_back(_chunkMap[location]);
			}
		}
	}

	cameraPosition = _renderPoint;
	std::sort(chunks.begin(), chunks.end(), [cameraPosition](const Chunk * a, const Chunk * b)
		{ return a->getDistance(cameraPosition) < b->getDistance(cameraPosition);});
	return chunks;
}

void World::_generateChunks()
{
	ChunkVec chunksToGenerate;
	std::mutex cvMutex;

	// Tip for tomorrow: maybe add a cv to wait for chunks to be built/meshed, then when they're all done, wake the loop and reset counter
	while (true)
	{
		Logger::log(VOXEL, DEBUG, "Waiting");
		std::unique_lock<std::mutex> lock(cvMutex);
		_cv.wait(lock, [&] { return !_isLoaded.load(std::memory_order_relaxed)
			|| (_isProceduralRequested.load(std::memory_order_relaxed) && !_isLocked.load(std::memory_order_relaxed)); });
		Logger::log(VOXEL, DEBUG, "Awake");
		if (!_isLoaded.load(std::memory_order_relaxed))
			return;
		if (_isLocked.load(std::memory_order_relaxed))
			continue;
		_isProceduralRequested.store(false);
		ChunkVec newChunks = _queryChunksInRange();
		if (newChunks.empty())
			continue;
		{
			std::lock_guard<std::mutex> lg(_visibleChunksMutex);
			_nextVisibleChunks = newChunks;
			_readyToSwap.store(true);
		}
		bool chunksReady;
		bool isProceduralRequested = false;
		bool isLoaded = true;
		do {
			chunksReady = true;
			for (Chunk * chunk : newChunks)
			{
				if (!chunk)
					continue;
				ChunkState state = chunk->getState();
				isProceduralRequested = _isProceduralRequested.load(std::memory_order_relaxed);
				isLoaded = _isLoaded.load(std::memory_order_relaxed);
				if (isProceduralRequested || !isLoaded)
					break;
				if (state == IDLE)
				{
					chunk->setState(BUILDING);
					_chunkPool.submitTask([chunk]() { chunk->build(); });
					chunksReady = false;
				}
				else if (state == BUILT && chunk->isReadyForMesh())
				{
					chunk->setState(MESHING);
					_chunkPool.submitTask([chunk]() { chunk->generateMesh(); });
					chunksReady = false;
				}
				else if (state == BUILDING || state == MESHING)
					chunksReady = false;
			}
			if (!chunksReady && !isProceduralRequested && isLoaded)
				std::this_thread::yield();
		} while (!chunksReady && !isProceduralRequested && isLoaded);
	}
}

void World::_checkForChunkDeletion(AEngine * engine, Camera * camera)
{
	const glm::vec3 & camPos = camera->getPosition();
	const uint8_t renderDistance = camera->getRenderDistance() + CHUNK_DELETION_DISTANCE;
	int renderDistanceNorth = camPos.x + renderDistance * CHUNK_WIDTH;
	int renderDistanceSouth = camPos.x - renderDistance * CHUNK_WIDTH;
	int renderDistanceEast = camPos.y + renderDistance * CHUNK_HEIGHT;
	int renderDistanceWest = camPos.y - renderDistance * CHUNK_HEIGHT;
	int renderDistanceUp = camPos.z + renderDistance * CHUNK_LENGTH;
	int renderDistanceDown = camPos.z - renderDistance * CHUNK_LENGTH;

	std::vector<glm::ivec3> iterators;
	std::lock_guard<std::mutex> lg(_mapMutex);
	for (const std::pair<const glm::ivec3, Chunk *> & chunkPair : _chunkMap)
	{
		Chunk * chunk = chunkPair.second;
		if (!chunk)
			continue;
		const glm::ivec3 & chunkPos = chunkPair.first;
		if (chunkPos.x * CHUNK_WIDTH > renderDistanceNorth || chunkPos.x * CHUNK_WIDTH < renderDistanceSouth
			|| chunkPos.y * CHUNK_HEIGHT > renderDistanceEast || chunkPos.y * CHUNK_HEIGHT < renderDistanceWest
			|| chunkPos.z * CHUNK_LENGTH > renderDistanceUp || chunkPos.z * CHUNK_LENGTH < renderDistanceDown)
		{
			if (chunk->getState() != UPLOADED)
				continue;
			Logger::log(VOXEL, WARNING, "Deleted chunk at: " + toString(chunkPos.x * CHUNK_WIDTH) + ", " + toString(chunkPos.y * CHUNK_HEIGHT)
				+ ", " + toString(chunkPos.z * CHUNK_LENGTH));
			if (chunk->unload(engine))
			{
				iterators.push_back(chunkPos);
				delete chunk;
				_chunkMap[chunkPos] = nullptr;
			}
		}
	}
	for (const glm::ivec3 & pos : iterators)
		_chunkMap.erase(pos);
}
