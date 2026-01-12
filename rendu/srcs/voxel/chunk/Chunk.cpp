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
	if (getState() != NONE)
		return;

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
	if (getState() != BUILT)
		return;

	Profiler p("Chunk::generateMesh");
	_generateGreedyMesh();
	setState(MESHED);
}

void Chunk::uploadAsset(AEngine * engine)
{
	std::lock_guard<std::mutex> lg(_workerMutex);
	if (getState() != MESHED)
		return;

	if (!_asset.vertices.empty())
	{
		Profiler p("Chunk::uploadAsset");
		engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_VOXEL));
	}
	setState(UPLOADED);
}

void Chunk::unload(AEngine * engine)
{
	std::lock_guard<std::mutex> lg(_workerMutex);
	if (getState() != UPLOADED)
		return;

	engine->unloadAsset(_asset.assetID);
	setState(MESHED);
}
