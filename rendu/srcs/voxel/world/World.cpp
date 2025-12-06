#include "World.hpp"
#include <algorithm>

World::~World()
{
	for (std::pair<glm::ivec3, Chunk *> chunks : _chunkMap)
		delete chunks.second;
}

void World::load()
{
	for (int x = 0; x < WORLD_WIDTH; ++x)
	{
		for (int y = 0; y < WORLD_HEIGHT; ++y)
		{
			for (int z = 0; z < WORLD_LENGTH; ++z)
			{
				Chunk * chunk = new Chunk(x, y, z, this);
				chunk->build();
				_chunkMap[chunk->getChunkLocation()] = chunk;
			}
		}
	}
	for (std::pair<glm::ivec3, Chunk *> chunkPtr : _chunkMap)
	{
		if (chunkPtr.second)
			chunkPtr.second->generateMesh();
	}
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

void World::reloadChunks()
{
	for (std::pair<glm::ivec3, Chunk *> chunks : _chunkMap)
	{
		Chunk * chunk = chunks.second;
		if (chunk && chunk->getState() == UPLOADED)
			chunk->unload();// bref
	}
}

void World::render(AEngine * engine)
{
	std::vector<Chunk *> visibleChunk = _generateVisibleChunk(engine->getCamera());

	_generateProceduralTerrain(visibleChunk);
	_generateProceduralMesh(visibleChunk);
	_uploadChunk(visibleChunk, engine);
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

void World::_generateProceduralTerrain(std::vector<Chunk *> visibleChunk)
{
	for (Chunk * chunk : visibleChunk)
	{
		if (chunk->getState() == NONE)
			chunk->build();
	}
}

void World::_generateProceduralMesh(std::vector<Chunk *> visibleChunk)
{
	for (Chunk * chunk : visibleChunk)
	{
		if (chunk && chunk->getState() == BUILT)
			chunk->generateMesh();
	}
}

void World::_uploadChunk(std::vector<Chunk *> visibleChunk, AEngine * engine)
{
	for (Chunk * chunk : visibleChunk)
	{
		if (chunk->getState() == MESHED)
			chunk->uploadAsset(engine);
		engine->drawAsset(chunk->getAsset().assetID);
	}
}
