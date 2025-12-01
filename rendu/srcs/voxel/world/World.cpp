#include "World.hpp"
#include <algorithm>

World::~World()
{
	for (std::pair<glm::ivec3, Chunk *> chunks : _chunkMap)
		delete chunks.second;
}

void World::load()
{
	for (int x = 0; x < 1; ++x)
	{
		for (int y = 0; y < 1; ++y)
		{
			for (int z = 0; z < 1; ++z)
			{
				Chunk * chunk = new Chunk(x, y, z);
				chunk->build();
				chunk->generateMesh();
				_chunkMap[chunk->getChunkLocation()] = chunk;
			}
		}
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
	for (std::pair<glm::ivec3, Chunk *> chunks : _chunkMap)
	{
		Chunk * chunk = chunks.second;
		if (chunk && chunk->getState() == MESHED)
			chunk->upload(engine);
		engine->drawAsset(chunk->getAsset().assetID);
	}
}
