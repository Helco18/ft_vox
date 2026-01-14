#include "World.hpp"
#include "Logger.hpp"
#include <algorithm>

World::~World()
{
	_chunkPool.stop();
	for (std::pair<glm::ivec3, Chunk *> chunks : _chunkMap)
		delete chunks.second;
}

void World::load()
{
	_chunkPool.start(std::thread::hardware_concurrency() - 1); // max thread minus one for main
}

void World::addChunk(Chunk * chunk)
{
	ChunkMap::iterator it = _chunkMap.find(chunk->getChunkLocation());
	if (it != _chunkMap.end())
	{
		Chunk * foundChunk = _chunkMap[chunk->getChunkLocation()];
		if (foundChunk)
			delete foundChunk;
	}
	_chunkMap[chunk->getChunkLocation()] = chunk;
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

void World::_generateVisibleChunks(Camera * camera)
{
	glm::vec3 cameraPosition = camera->getPosition();
	cameraPosition.x /= CHUNK_WIDTH;
	cameraPosition.y /= CHUNK_HEIGHT;
	cameraPosition.z /= CHUNK_LENGTH;
	int renderDistance = (camera->getRenderDistance());
	int renderDistanceNorth = renderDistance + cameraPosition.x;
	int renderDistanceSouth = cameraPosition.x - renderDistance;
	int renderDistanceEast = renderDistance + cameraPosition.z;
	int renderDistanceWest = cameraPosition.z - renderDistance;
	int renderDistanceUp = renderDistance + cameraPosition.y;
	int renderDistanceDown = cameraPosition.y - renderDistance;

	for (int x = renderDistanceSouth; x < renderDistanceNorth; ++x)
	{
		for (int y = renderDistanceDown; y < renderDistanceUp; ++y)
		{
			for (int z = renderDistanceWest; z < renderDistanceEast; ++z)
			{
				if (_chunkMap[glm::vec3(x, y, z)])
				{
					_visibleChunks.push_back(_chunkMap[glm::vec3(x, y, z)]);
					continue;
				}
				Chunk * chunk = new Chunk(x, y, z, this);
				_chunkMap[chunk->getChunkLocation()] = chunk;
				_visibleChunks.push_back(_chunkMap[chunk->getChunkLocation()]);
			}
		}
	}
}

void World::_generateProceduralTerrain()
{
	for (Chunk * chunk : _visibleChunks)
	{
		if (chunk && chunk->getState() == NONE)
		{
			chunk->setState(BUILDING);
			_chunkPool.submitTask([chunk]() {chunk->build();});
		}
	}
}

void World::_generateProceduralMesh()
{
	for (Chunk * chunk : _visibleChunks)
	{
		if (chunk && chunk->getState() == BUILT)
		{
			chunk->setState(MESHING);
			_chunkPool.submitTask([chunk]() {chunk->generateMesh();});
		}
	}
}

void World::_generateChunks()
{
	_generateProceduralTerrain();

	bool allChunksBuilt = false;
	do
	{
		allChunksBuilt = true;
		for (Chunk * chunk : _visibleChunks)
		{
			if (chunk->getState() < BUILT)
			{
				allChunksBuilt = false;
				break;
			}
		}
	} while (!allChunksBuilt);

	_generateProceduralMesh();
}

void World::generateProcedurally(Camera * camera)
{
	static Chunk * lastVisitedChunk = nullptr;
	Chunk * currentChunk = getChunkAt(camera->getPosition());

	if (lastVisitedChunk == currentChunk && lastVisitedChunk)
		return;
	lastVisitedChunk = currentChunk;
	_visibleChunks.clear();
	_generateVisibleChunks(camera);
	if (lastVisitedChunk)
		_chunkPool.submitTask([this]() { _generateChunks(); });
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
