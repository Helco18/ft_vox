#include "Chunk.hpp"
#include "utils.hpp"
#include "BlockData.hpp"

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

	// SOUTH
	if (x > 0)
		neighboringBlocks.push_back(_blocks[x - 1][y][z]);
	else
		neighboringBlocks.push_back(0);
	// NORTH
	if (x + 1 < CHUNK_WIDTH)
		neighboringBlocks.push_back(_blocks[x + 1][y][z]);
	else
		neighboringBlocks.push_back(0);

	// WEST
	if (z > 0)
		neighboringBlocks.push_back(_blocks[x][y][z - 1]);
	else
		neighboringBlocks.push_back(0);
	// EAST
	if (z + 1 < CHUNK_LENGTH)
		neighboringBlocks.push_back(_blocks[x][y][z + 1]);
	else
		neighboringBlocks.push_back(0);

	// TOP
	if (y > 0)
		neighboringBlocks.push_back(_blocks[x][y - 1][z]);
	else
		neighboringBlocks.push_back(0);
	// BOTTOM
	if (y + 1 < CHUNK_WIDTH)
		neighboringBlocks.push_back(_blocks[x][y + 1][z]);
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

void Chunk::upload(AEngine * engine)
{
	engine->upload(_asset);
	_state = UPLOADED;
}

void Chunk::unload()
{
	_state = MESHED;
}
