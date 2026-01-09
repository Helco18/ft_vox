#include "Chunk.hpp"
#include "utils.hpp"
#include "World.hpp"
#include "Logger.hpp"
#include "TextureAtlas.hpp"
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
				if (((y + (_chunkLocation.y * CHUNK_HEIGHT)) <= (-1 + (std::sin(((_chunkLocation.x * CHUNK_WIDTH) + x) / 5.0) * 5.0 + (std::cos(((_chunkLocation.z * CHUNK_LENGTH) + z) / 5.0) * 5.0))) ))
				{
					_blocks[x][y][z] = x % 2 + 1;
					// _blocks[x][y][z] = 1;
				}
				else
					_blocks[x][y][z] = 0;
			}
		}
	}
	_state = BUILT;
}

void Chunk::generateMesh()
{
	_generateGreedyMesh();
	_state = MESHED;
}

void Chunk::uploadAsset(AEngine * engine)
{
	if (!_asset.vertices.empty())
		engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_VOXEL));
	_state = UPLOADED;
}

void Chunk::unload()
{
	_state = MESHED;
}
