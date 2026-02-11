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

void World::load()
{
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
		std::unique_lock<std::mutex> lock(cvMutex);
		_cv.wait(lock, [&] { return !_isLoaded.load(std::memory_order_relaxed)
			|| (_isProceduralRequested.load(std::memory_order_relaxed) && !_isLocked.load(std::memory_order_relaxed)); });
		if (!_isLoaded.load(std::memory_order_relaxed))
			return;
		if (_isLocked.load(std::memory_order_relaxed))
			continue;
		_isProceduralRequested.store(false);
		ChunkVec newChunks = _queryChunksInRange();
		{
			std::lock_guard<std::mutex> lg(_visibleChunksMutex);
			_nextVisibleChunks = newChunks;
			std::sort(_nextVisibleChunks.begin(), _nextVisibleChunks.end(), [this](const Chunk * a, const Chunk * b)
				{ return a->getDistance(_renderPoint) > b->getDistance(_renderPoint);});
			_readyToSwap.store(true);
		}
		if (newChunks.empty())
			continue;
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
				if (state == NONE)
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
				chunk = nullptr;
			}
		}
	}
	for (const glm::ivec3 & pos : iterators)
		_chunkMap.erase(pos);
}

void World::update(AEngine * , Camera * camera)
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
		// _checkForChunkDeletion(engine, camera);
	}
	_renderPoint = camPos;
	_isProceduralRequested.store(true);
	_cv.notify_one();
}

void World::_extractPlanesFromProjmat(Camera * camera)
{
	glm::mat4 projmat = camera->getBuffer().proj * camera->getBuffer().view;
	for (int f = 0; f <= top; ++f)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (f == right)
				_planes[f].plane[i] = projmat[i][3] + projmat[i][0];
			if (f == left)
				_planes[f].plane[i] = projmat[i][3] - projmat[i][0];
			if (f == top)
				_planes[f].plane[i] = projmat[i][3] + projmat[i][1];
			if (f == bottom)
				_planes[f].plane[i] = projmat[i][3] - projmat[i][1];
		}
		glm::vec3 normal(_planes[f].plane[0], _planes[f].plane[1], _planes[f].plane[2]);
		float len = length(normal);
		_planes[f].plane[0] /= len;
		_planes[f].plane[1] /= len;
		_planes[f].plane[2] /= len;
		_planes[f].plane[3] /= len;
	}
}

float getSignedDistanceToPlane(glm::vec3 pos, plane p)
{
	glm::vec3 normal(p.plane[0], p.plane[1], p.plane[2]);
	return(glm::dot(normal, pos) + (p.plane[3]));
}

bool World::_chunkIsFrustum(Chunk * chunk)
{
	glm::vec3 pos = chunk->getChunkLocation();
	pos.x = pos.x * CHUNK_WIDTH + static_cast<float>(CHUNK_WIDTH) / 2;
	pos.y = pos.y * CHUNK_HEIGHT + static_cast<float>(CHUNK_HEIGHT) / 2;
	pos.z = pos.z * CHUNK_LENGTH + static_cast<float>(CHUNK_LENGTH) / 2;
	if (getSignedDistanceToPlane(pos, _planes[right]) > 0 &&
		getSignedDistanceToPlane(pos, _planes[left]) > 0 &&
		getSignedDistanceToPlane(pos, _planes[bottom]) > 0 &&
		getSignedDistanceToPlane(pos, _planes[top]) > 0)
	{
		return true;
	}
	return false;
}

void World::render(AEngine * engine, PipelineType pipelineType, Camera * camera)
{
	(void) camera;
	if (_readyToSwap)
	{
		std::lock_guard<std::mutex> lg(_visibleChunksMutex);
		_visibleChunks = _nextVisibleChunks;
		_nextVisibleChunks.clear();
		_readyToSwap.store(false);
	}
	// _extractPlanesFromProjmat(camera);
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
		else if (state == UPLOADED)
			chunk->drawAsset(engine, pipelineType);
	}
}
