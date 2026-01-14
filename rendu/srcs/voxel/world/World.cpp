#include "World.hpp"
#include "Logger.hpp"
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

Chunk * World::getChunkAt(int x, int y, int z) const
{
	int chunkX;
	int chunkY;
	int chunkZ;

	chunkX = static_cast<int>(std::floor(static_cast<double>(x) / CHUNK_WIDTH));
	chunkY = static_cast<int>(std::floor(static_cast<double>(y) / CHUNK_HEIGHT));
	chunkZ = static_cast<int>(std::floor(static_cast<double>(z) / CHUNK_LENGTH));
	return getChunkAtChunkLocation(chunkX, chunkY, chunkZ);
}

Chunk * World::getChunkAtChunkLocation(int x, int y, int z) const
{
	auto it = _chunkMap.find(glm::ivec3(x, y, z));
	if (it != _chunkMap.end())
		return it->second;
	return nullptr;
}

inline Chunk * World::getChunkAt(const glm::vec3 & location) const
{
	return getChunkAt(location.x, location.y, location.z);
}

inline Chunk * World::getChunkAtChunkLocation(const glm::vec3 & location) const
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

static bool isWithinRenderDistance(Chunk * chunk, Camera * camera)
{
	int renderDistanceMin = getRenderDistanceMin();
	int ratioW = CHUNK_WIDTH / renderDistanceMin;
	int ratioH = CHUNK_HEIGHT / renderDistanceMin;
	int ratioL = CHUNK_LENGTH / renderDistanceMin;
	int renderDistance = (camera->getRenderDistance());
	int renderDistanceX = (renderDistance / ratioW) == 0 ? 1 : (renderDistance / ratioW);
	int renderDistanceY = (renderDistance / ratioH) == 0 ? 1 : renderDistance / ratioH;
	int renderDistanceZ = (renderDistance / ratioL) == 0 ? 1 : (renderDistance / ratioL);

	return !(chunk->getChunkX() > static_cast<int>(std::floor(camera->getPosition().x / CHUNK_WIDTH)) + renderDistanceX
	|| chunk->getChunkX() < static_cast<int>(std::floor(camera->getPosition().x / CHUNK_WIDTH)) - renderDistanceX
	|| chunk->getChunkY() > static_cast<int>(std::floor(camera->getPosition().y / CHUNK_HEIGHT)) + renderDistanceY
	|| chunk->getChunkY() < static_cast<int>(std::floor(camera->getPosition().y / CHUNK_HEIGHT)) - renderDistanceY
	|| chunk->getChunkZ() < static_cast<int>(std::floor(camera->getPosition().z / CHUNK_LENGTH)) - renderDistanceZ
	|| chunk->getChunkZ() < static_cast<int>(std::floor(camera->getPosition().z / CHUNK_LENGTH)) - renderDistanceZ);
}

World::VisibleChunks World::_generateVisibleChunks(Camera * camera)
{
	VisibleChunks visibleChunks;
	glm::vec3 cameraPosition = camera->getPosition();
	int renderDistanceMin = getRenderDistanceMin();
	cameraPosition.x /= CHUNK_WIDTH;
	cameraPosition.y /= CHUNK_HEIGHT;
	cameraPosition.z /= CHUNK_LENGTH;

	int ratioW = CHUNK_WIDTH / renderDistanceMin;
	int ratioH = CHUNK_HEIGHT / renderDistanceMin;
	int ratioL = CHUNK_LENGTH / renderDistanceMin;

	int renderDistance = (camera->getRenderDistance());
	int renderDistanceX = (renderDistance / ratioW) == 0 ? 1 : (renderDistance / ratioW);
	int renderDistanceY = (renderDistance / ratioH) == 0 ? 1 : renderDistance / ratioH;
	int renderDistanceZ = (renderDistance / ratioL) == 0 ? 1 : (renderDistance / ratioL);

	int renderDistanceNorth = renderDistanceX + cameraPosition.x;
	int renderDistanceSouth = cameraPosition.x - renderDistanceX;
	int renderDistanceEast = renderDistanceZ + cameraPosition.z;
	int renderDistanceWest = cameraPosition.z - renderDistanceZ;
	int renderDistanceUp = renderDistanceY + cameraPosition.y;
	int renderDistanceDown = cameraPosition.y - renderDistanceY;

	for (int x = renderDistanceSouth; x < renderDistanceNorth; ++x)
	{
		for (int y = renderDistanceDown; y < renderDistanceUp; ++y)
		{
			for (int z = renderDistanceWest; z < renderDistanceEast; ++z)
			{
				glm::vec3 location(x, y, z);
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

void World::_generateProceduralTerrain(Camera * camera)
{
	for (Chunk * chunk : _visibleChunks)
	{
		if (!chunk || !isWithinRenderDistance(chunk, camera))
			continue;
		if (chunk->getState() == NONE)
		{
			chunk->setState(BUILDING);
			_chunkPool.submitTask([chunk]() {chunk->build();});
		}
	}
}

void World::_generateProceduralMesh(Camera * camera)
{
	for (Chunk * chunk : _visibleChunks)
	{
		if (!chunk || !isWithinRenderDistance(chunk, camera))
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
		_generateProceduralTerrain(camera);

		bool allChunksBuilt = false;
		do
		{
			allChunksBuilt = true;
			for (Chunk * chunk : _visibleChunks)
			{
				if (_isProceduralRequested)
					break;
				if (!chunk || !isWithinRenderDistance(chunk, camera))
					continue;
				if (chunk->getState() < BUILT)
				{
					allChunksBuilt = false;
					break;
				}
			}
		} while (!allChunksBuilt && !_isProceduralRequested);

		_generateProceduralMesh(camera);
		_visibleChunks = visibleChunks;
	}
}

void World::generateProcedurally(Camera * camera)
{
	static Chunk * lastVisitedChunk = nullptr;
	Chunk * currentChunk = getChunkAt(camera->getPosition());

	if (lastVisitedChunk == currentChunk && lastVisitedChunk)
		return;
	lastVisitedChunk = currentChunk;
	_isProceduralRequested = true;
	if (!_isLoaded.load())
	{
		_isLoaded.store(true);
		_chunkPool.submitTask([this, camera]() { _generateChunks(camera); });
	}
	_chunkCv.notify_all();
}

void World::render(AEngine * engine, PipelineType pipelineType)
{
	for (Chunk * chunk : _visibleChunks)
	{
		if (chunk->getState() == MESHED)
			chunk->uploadAsset(engine);
		else if (chunk->getState() == UPLOADED)
			engine->drawAsset(chunk->getAsset().assetID, PipelineManager::getPipeline(pipelineType));
	}
}
