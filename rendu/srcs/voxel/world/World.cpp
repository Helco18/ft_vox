#include "World.hpp"
#include <algorithm>

World::~World()
{
	_chunkPool.stop();
	for (std::pair<glm::ivec3, Chunk *> chunks : _chunkMap)
		delete chunks.second;
}

void World::load()
{
	_chunkPool.start(std::thread::hardware_concurrency()); // max thread
}

Chunk * World::getChunk(const glm::vec3 & location)
{
	int floorX = std::floor(location.x);
	int floorY = std::floor(location.y);
	int floorZ = std::floor(location.z);

	glm::ivec3 flooredLocation(floorX, floorY, floorZ);
	ChunkMap::iterator it = _chunkMap.find(flooredLocation);
	if (it != _chunkMap.end())
		return it->second;
	return nullptr;
}

Chunk * World::getChunk(int x, int y, int z)
{
	return getChunk(glm::ivec3(x, y, z));
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

void World::render(AEngine * engine, PipelineType pipelineType)
{
	std::vector<Chunk *> visibleChunk = _generateVisibleChunk(engine->getCamera());

	_generateProceduralTerrain(visibleChunk);
	_generateProceduralMesh(visibleChunk);
	_drawChunk(visibleChunk, engine, pipelineType);
}

std::vector<Chunk *> World::_generateVisibleChunk(Camera * camera)
{
	std::vector<Chunk *> visibleChunk;
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
					visibleChunk.push_back(_chunkMap[glm::vec3(x, y, z)]);
					continue;
				}
				Chunk * chunk = new Chunk(x, y, z, this);
				_chunkMap[chunk->getChunkLocation()] = chunk;
				visibleChunk.push_back(_chunkMap[chunk->getChunkLocation()]);
			}
		}
	}
	return visibleChunk;
}

void World::_generateProceduralTerrain(const std::vector<Chunk *> & visibleChunk)
{
	for (Chunk * chunk : visibleChunk)
	{
		if (chunk && chunk->getState() == NONE)
			_chunkPool.submitTask([chunk]() {chunk->build();});
	}
}

void World::_generateProceduralMesh(const std::vector<Chunk *> & visibleChunk)
{
	for (Chunk * chunk : visibleChunk)
	{
		if (chunk && chunk->getState() == BUILT)
			_chunkPool.submitTask([chunk]() {chunk->generateMesh();});
	}
}

void World::_drawChunk(const std::vector<Chunk *> & visibleChunk, AEngine * engine, PipelineType pipelineType)
{
	for (Chunk * chunk : visibleChunk)
	{
		if (chunk->getState() == MESHED)
			chunk->uploadAsset(engine);
		if (chunk->getState() == UPLOADED)
			engine->drawAsset(chunk->getAsset().assetID, PipelineManager::getPipeline(pipelineType));
	}
}
