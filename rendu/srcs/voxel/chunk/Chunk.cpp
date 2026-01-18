#include "Chunk.hpp"
#include "Profiler.hpp"
#include "utils.hpp"
#include "World.hpp"
#include "Logger.hpp"
#include "TextureAtlas.hpp"
#include <iostream>

void Chunk::build()
{
	std::lock_guard<std::mutex> lg(_workerMutex);

	Profiler p("Chunk::build");
	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int y = 0; y < CHUNK_HEIGHT; ++y)
		{
			for (int z = 0; z < CHUNK_LENGTH; ++z)
			{
				if (((y + (_chunkLocation.y * CHUNK_HEIGHT)) <= (-1 + (std::sin(((_chunkLocation.x * CHUNK_WIDTH) + x) / 5.0) * 5.0 +
						(std::cos(((_chunkLocation.z * CHUNK_LENGTH) + z) / 5.0) * 5.0))) ))
				{
					_blocks[x][y][z] = x % 2 + 1;
					// _blocks[x][y][z] = 1;
				}
				else
					_blocks[x][y][z] = 0;
			}
		}
	}
	setState(BUILT);
}

void Chunk::generateMesh()
{
	std::lock_guard<std::mutex> lg(_workerMutex);

	Profiler p("Chunk::generateMesh");
	_generateGreedyMesh();
	if (!_asset.vertices.bytes.empty() && !_asset.indices.empty())
		setState(MESHED);
	else
		setState(MESHED_EMPTY);
}

void Chunk::uploadAsset(AEngine * engine)
{
	std::lock_guard<std::mutex> lg(_workerMutex);
 
	Profiler p("Chunk::uploadAsset");
	engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_VOXEL).id);
	setState(UPLOADED);
}

void Chunk::drawAsset(AEngine * engine, PipelineType pipelineType)
{
	engine->drawAsset(_asset.assetID, PipelineManager::getPipeline(pipelineType).id);
}

void Chunk::unload(AEngine * engine)
{
	std::lock_guard<std::mutex> lg(_workerMutex);

	engine->unloadAsset(_asset.assetID);
	setState(MESHED);
}

ChunkState Chunk::getState()
{
	const std::lock_guard<std::mutex> lg(_stateMutex);
	return _state;
}

void Chunk::setState(ChunkState state)
{
	const std::lock_guard<std::mutex> lg(_stateMutex);
	_state = state;
}
