#include "World.hpp"
#include "Logger.hpp"
#include "Profiler.hpp"
#include <algorithm>

World::~World()
{
	_isLoaded.store(false);
	_chunkCv.notify_all();
	_chunkPool.stop();
	for (std::pair<glm::ivec3, Chunk *> chunks : _chunkMap)
		delete chunks.second;
}

void World::load()
{
	_chunkPool.start(std::thread::hardware_concurrency() - 1); // max thread minus one for main
}

void World::reloadChunks(AEngine * engine)
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

void World::_computeRenderDistance(Camera * camera)
{
	int renderDistanceMin = getRenderDistanceMin();

	int ratioW = CHUNK_WIDTH / renderDistanceMin;
	int ratioH = CHUNK_HEIGHT / renderDistanceMin;
	int ratioL = CHUNK_LENGTH / renderDistanceMin;

	int renderDistance = (camera->getRenderDistance());
	_renderDistanceX = (renderDistance / ratioW) == 0 ? 1 : (renderDistance / ratioW);
	_renderDistanceY = (renderDistance / ratioH) == 0 ? 1 : renderDistance / ratioH;
	_renderDistanceZ = (renderDistance / ratioL) == 0 ? 1 : (renderDistance / ratioL);
}

bool World::_isWithinRenderDistance(Chunk * chunk, Camera * camera)
{
	return !(chunk->getChunkX() > static_cast<int>(std::floor(camera->getPosition().x / CHUNK_WIDTH)) + _renderDistanceX
		|| chunk->getChunkX() < static_cast<int>(std::floor(camera->getPosition().x / CHUNK_WIDTH)) - _renderDistanceX
		|| chunk->getChunkY() > static_cast<int>(std::floor(camera->getPosition().y / CHUNK_HEIGHT)) + _renderDistanceY
		|| chunk->getChunkY() < static_cast<int>(std::floor(camera->getPosition().y / CHUNK_HEIGHT)) - _renderDistanceY
		|| chunk->getChunkZ() < static_cast<int>(std::floor(camera->getPosition().z / CHUNK_LENGTH)) - _renderDistanceZ
		|| chunk->getChunkZ() < static_cast<int>(std::floor(camera->getPosition().z / CHUNK_LENGTH)) - _renderDistanceZ);
}

World::VisibleChunks World::_generateVisibleChunks(Camera * camera)
{
	VisibleChunks visibleChunks;
	glm::vec3 cameraPosition = camera->getPosition();
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
					visibleChunks.push_back(it->second);
					continue;
				}
				Chunk * chunk = new Chunk(x, y, z, this);
				_chunkMap.try_emplace(location, chunk);
				visibleChunks.push_back(chunk);
			}
		}
	}
	return visibleChunks;
}

void World::_generateProceduralTerrain(Camera * camera, VisibleChunks & visibleChunks)
{
	for (Chunk * chunk : visibleChunks)
	{
		if (!chunk || !_isWithinRenderDistance(chunk, camera))
			continue;
		if (chunk->getState() == NONE)
		{
			chunk->setState(BUILDING);
			_chunkPool.submitTask([chunk]() {chunk->build();});
		}
	}
}

void World::_generateProceduralMesh(Camera * camera, VisibleChunks & visibleChunks)
{
	for (Chunk * chunk : visibleChunks)
	{
		if (!chunk || !_isWithinRenderDistance(chunk, camera))
			continue;
		if (chunk->getState() == BUILT)
		{
			chunk->setState(MESHING);
			_chunkPool.submitTask([chunk]() {chunk->generateMesh();});
		}
	}
}

void World::_generateChunks(Camera * camera)
{
	while (_isLoaded.load())
	{
		std::unique_lock<std::mutex> lock(_chunkMutex);
		_chunkCv.wait(lock, [&] { return !_isLoaded.load() || _isProceduralRequested; });
		if (!_isLoaded.load())
			return;
		_isProceduralRequested = false;

		VisibleChunks visibleChunks = _generateVisibleChunks(camera);
		_generateProceduralTerrain(camera, visibleChunks);

		bool allChunksBuilt = false;
		do
		{
			allChunksBuilt = true;
			for (Chunk * chunk : visibleChunks)
			{
				if (_isProceduralRequested)
					break;
				if (!chunk || !_isWithinRenderDistance(chunk, camera))
					continue;
				if (chunk->getState() < BUILT)
				{
					allChunksBuilt = false;
					break;
				}
			}
		} while (!allChunksBuilt && !_isProceduralRequested);

		_generateProceduralMesh(camera, visibleChunks);
		std::lock_guard<std::mutex> lg(_mapMutex);
		_visibleChunks = visibleChunks;
	}
}

static glm::ivec3 posToChunkPos(glm::vec3 pos)
{
	glm::vec3 chunkPos;
	chunkPos.x = static_cast<int>(std::floor(static_cast<double>(pos.x) / CHUNK_WIDTH));
	chunkPos.y = static_cast<int>(std::floor(static_cast<double>(pos.y) / CHUNK_HEIGHT));
	chunkPos.z = static_cast<int>(std::floor(static_cast<double>(pos.z) / CHUNK_LENGTH));
	return pos;
}

void World::generateProcedurally(Camera * camera)
{
	static bool firstLoad = true;
	static glm::ivec3 lastVisitedChunk(0, 0, 0);
	glm::ivec3 currentChunk = posToChunkPos(camera->getPosition());

	static int oldRenderDistance = 0;
	int renderDistance = (camera->getRenderDistance());

	if (oldRenderDistance != renderDistance)
	{
		_computeRenderDistance(camera);
		oldRenderDistance = renderDistance;
		_isProceduralRequested = true;
		_chunkCv.notify_all();
		return;
	}

	if (lastVisitedChunk == currentChunk && !firstLoad)
		return;
	lastVisitedChunk = currentChunk;
	_isProceduralRequested = true;
	if (firstLoad)
	{
		firstLoad = false;
		_isLoaded.store(true);
		_chunkPool.submitTask([this, camera]() { _generateChunks(camera); });
	}
	_chunkCv.notify_all();
}

void World::render(AEngine * engine, PipelineType pipelineType)
{
	std::lock_guard<std::mutex> lg(_mapMutex);
	for (Chunk * chunk : _visibleChunks)
	{
		ChunkState state = chunk->getState();
		if (state == MESHED)
			chunk->uploadAsset(engine);
		else if (state == UPLOADED)
			chunk->drawAsset(engine, pipelineType);
	}
}
