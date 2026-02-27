#include "Profiler.hpp"
#include "World.hpp"

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
		for (int y = renderDistanceUp; y > renderDistanceDown; --y)
		{
			for (int z = renderDistanceWest; z < renderDistanceEast; ++z)
			{
				if (!_isLoaded.load(std::memory_order_relaxed))
					return {};
				glm::ivec3 location(x, y, z);
				std::lock_guard<std::mutex> lg(_mapMutex);
				ChunkMap::iterator it = _chunkMap.find(location);
				if (it != _chunkMap.end())
				{
					chunks.push_back(it->second);
					continue;
				}
				Chunk * chunk = new Chunk(x, y, z, this);
				_chunkMap.try_emplace(location, std::move(chunk));
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
	std::mutex cvMutex;

	while (true)
	{
		std::unique_lock<std::mutex> lock(cvMutex);
		_cv.wait(lock, [&] { return !_isLoaded.load(std::memory_order_relaxed)
			|| (_isProceduralRequested.load(std::memory_order_relaxed) && !_isLocked.load(std::memory_order_relaxed)); });
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
		std::vector<Task> taskList;
		taskList.reserve(newChunks.size());
		do {
			chunksReady = true;
			for (Chunk * chunk : newChunks)
			{
				if (!_isLoaded.load(std::memory_order_relaxed))
					return;
				isProceduralRequested = _isProceduralRequested.load(std::memory_order_relaxed);
				if (isProceduralRequested)
				{
					_chunkPool.clearTasks();
					for (Chunk * chunk : newChunks)
					{
						if (!chunk || chunk->isTakenByWorker())
							continue;
						ChunkState state = chunk->getState();
						if (state == BUILDING)
							chunk->setState(IDLE);
						else if (state == MESHING)
							chunk->setState(BUILT);
					}
					break;
				}
				if (!chunk || chunk->isTakenByWorker())
					continue;
				ChunkState state = chunk->getState();
				if (state == IDLE)
				{
					chunk->setState(BUILDING);
					taskList.push_back([chunk]() { chunk->build(); });
					chunksReady = false;
				}
				else if (state == BUILT && chunk->isReadyForMesh())
				{
					chunk->setState(MESHING);
					taskList.push_back([chunk]() { chunk->generateMesh(); });
					chunksReady = false;
				}
				else if (state == BUILDING || state == MESHING)
					chunksReady = false;
			}
			if (!chunksReady && !isProceduralRequested && taskList.empty())
				std::this_thread::yield();
			_chunkPool.submitBatch(taskList);
			taskList.clear();
		} while (!chunksReady && !isProceduralRequested);
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

	std::vector<Chunk *> chunksToDelete;
	std::vector<Chunk *> chunksToUnload;
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
			if (chunk->isTakenByWorker())
				continue;
			ChunkState state = chunk->getState();
			if (state == UPLOADED)
				chunksToUnload.push_back(chunk);
			chunksToDelete.push_back(chunk);
		}
	}
	for (Chunk * chunk : chunksToUnload)
		chunk->unload(engine);
	for (Chunk * chunk : chunksToDelete)
	{
		_chunkMap.erase(chunk->getChunkLocation());
		delete chunk;
	}
}
