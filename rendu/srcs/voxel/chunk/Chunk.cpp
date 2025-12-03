#include "Chunk.hpp"
#include "utils.hpp"
#include "BlockData.hpp"
#include "World.hpp"
#include <iostream>

void Chunk::build()
{
	if (_state != NONE)
		return;

	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int y = 0; y < CHUNK_HEIGHT; ++y)
		{
			for (int z = 0; z < CHUNK_LENGTH; ++z)
			{
				if (y <= 64)
					_blocks[x][y][z] = 1;
				else
					_blocks[x][y][z] = 0;
			}
		}
	}
	_state = BUILT;
}


std::vector<uint8_t> Chunk::_getNeighboringBlocks(int x, int y, int z)
{
	std::vector<uint8_t> neighboringBlocks;

	Chunk * northChunk = _world->getChunk(_chunkLocation.x + 1, _chunkLocation.y, _chunkLocation.z);
	Chunk * southChunk = _world->getChunk(_chunkLocation.x - 1, _chunkLocation.y, _chunkLocation.z);
	Chunk * eastChunk = _world->getChunk(_chunkLocation.x, _chunkLocation.y, _chunkLocation.z + 1);
	Chunk * westChunk = _world->getChunk(_chunkLocation.x, _chunkLocation.y, _chunkLocation.z - 1);
	Chunk * topChunk = _world->getChunk(_chunkLocation.x, _chunkLocation.y + 1, _chunkLocation.z);
	Chunk * bottomChunk = _world->getChunk(_chunkLocation.x, _chunkLocation.y - 1, _chunkLocation.z);

	// SOUTH
	if (x > 0)
		neighboringBlocks.push_back(_blocks[x - 1][y][z]);
	else if (southChunk)
		neighboringBlocks.push_back(southChunk->getBlock(CHUNK_WIDTH - 1 , y, z));
	else
		neighboringBlocks.push_back(0);
	// NORTH
	if (x + 1 < CHUNK_WIDTH)
		neighboringBlocks.push_back(_blocks[x + 1][y][z]);
	else if (northChunk)
		neighboringBlocks.push_back(northChunk->getBlock(0 , y, z));
	else
		neighboringBlocks.push_back(0);

	// WEST
	if (z > 0)
		neighboringBlocks.push_back(_blocks[x][y][z - 1]);
	else if (westChunk)
		neighboringBlocks.push_back(westChunk->getBlock(x , y, CHUNK_LENGTH - 1));
	else
		neighboringBlocks.push_back(0);
	// EAST
	if (z + 1 < CHUNK_LENGTH)
		neighboringBlocks.push_back(_blocks[x][y][z + 1]);
	else if (eastChunk)
		neighboringBlocks.push_back(eastChunk->getBlock(x , y, 0));
	else
		neighboringBlocks.push_back(0);

	// BOTTOM
	if (y > 0)
		neighboringBlocks.push_back(_blocks[x][y - 1][z]);
	else if (bottomChunk)
		neighboringBlocks.push_back(bottomChunk->getBlock(x ,0 , z));
	else
		neighboringBlocks.push_back(0);
	// TOP
	if (y + 1 < CHUNK_WIDTH)
		neighboringBlocks.push_back(_blocks[x][y + 1][z]);
	else if (topChunk)
		neighboringBlocks.push_back(topChunk->getBlock(x , CHUNK_WIDTH - 1, z));
	else
		neighboringBlocks.push_back(0);

	return neighboringBlocks;
}

bool Chunk::_blockIsVisible(int x, int y, int z)
{
	std::vector<uint8_t> neighboringBlocks = _getNeighboringBlocks(x, y, z);

	for (std::vector<uint8_t>::iterator it = neighboringBlocks.begin(); it != neighboringBlocks.end(); ++it)
	{
		BlockData blockData = BlockData::getBlockData(*it);
		if (!blockData.isVisible())
			return true;
	}
	return false;
}

void Chunk::generateMesh()
{
	const OBJModel model = OBJModel::getModel(CUBE);

	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int y = 0; y < CHUNK_HEIGHT; ++y)
		{
			for (int z = 0; z < CHUNK_LENGTH; ++z)
			{
				if (!_blockIsVisible(x, y, z))
					continue;
				// for (int i = 0; i <= BlockFace::BOTTOM; ++i)
				// {
				// 	for (int j = 0; j < 4; ++j)
				// 	{
				// 		_asset.vertices.push_back()
				// 	}
				// }

				uint32_t verticesAdded = _asset.vertices.size();
				if (_blocks[x][y][z] == 1)
				{
					for (Vertex vertex : model.getVertices())
					{
						Vertex tmp = vertex;
						tmp.position.x += x + _chunkLocation.x * CHUNK_WIDTH;
						tmp.position.y += y + _chunkLocation.y * CHUNK_HEIGHT;
						tmp.position.z += z + _chunkLocation.z * CHUNK_LENGTH;
						_asset.vertices.push_back(tmp);
					}
					for (uint32_t indice : model.getIndices())
						_asset.indices.push_back(indice + verticesAdded);
				}
			}
		}
	}
	_state = MESHED;
}

void Chunk::uploadAsset(AEngine * engine)
{
	engine->uploadAsset(_asset);
	_state = UPLOADED;
}

void Chunk::unload()
{
	_state = MESHED;
}
