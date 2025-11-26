#include "World.hpp"

void World::load()
{
	_chunkMap[glm::ivec3(0)] = new Chunk();
	_chunkMap[glm::ivec3(0)]->build();
	_chunkMap[glm::ivec3(0)]->generateMesh();
	_chunkMap[glm::ivec3(0)]->upload();
}

Chunk * World::getChunk(const glm::vec3 & location)
{
	int floorX = std::floor(location.x);
	int floorY = std::floor(location.y);
	int floorZ = std::floor(location.z);

	glm::ivec3 flooredLocation(floorX, floorY, floorZ);
	ChunkMap::iterator it = std::find(_chunkMap.begin(), _chunkMap.end(), flooredLocation);
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
	ChunkMap::iterator it = std::find(_chunkMap.begin(), _chunkMap.end(), chunk->getChunkLocation());
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
			chunk->unload();
	}
}
