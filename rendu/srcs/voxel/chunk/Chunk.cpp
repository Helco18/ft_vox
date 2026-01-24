#include "Chunk.hpp"
#include "Profiler.hpp"
#include "WindowManager.hpp"
#include "utils.hpp"
#include "World.hpp"
#include "Logger.hpp"
#include "TextureAtlas.hpp"
#include <iostream>

Chunk::Chunk(int x, int y, int z, World * world): _world(world), _chunkLocation(glm::ivec3(x, y, z)), _state(NONE)
{
	_asset.uniforms = &_chunkData;
}

void Chunk::build()
{
	std::lock_guard<std::mutex> lg(_workerMutex);

	if (!_world->isLoaded())
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
				}
				else if (y + (_chunkLocation.y * CHUNK_HEIGHT) <= 0)
				{
					_blocks[x][y][z] = 3;
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

	if (!_world->isLoaded())
		return;
	Profiler p("Chunk::generateMesh");
	_generateGreedyMesh();
	if (_asset.vertices.data && !_asset.indices.empty())
		setState(MESHED);
	else
		setState(MESHED_EMPTY);
}

void Chunk::uploadAsset(AEngine * engine)
{
	std::lock_guard<std::mutex> lg(_workerMutex);
 
	Profiler p("Chunk::uploadAsset");
	engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_VOXEL).id);
	engine->uploadAsset(_assetFrame, PipelineManager::getPipeline(PIPELINE_LINES).id);
	setState(UPLOADED);
}

void Chunk::drawAsset(AEngine * engine, PipelineType pipelineType)
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(engine->getWindow()));
	if (_chunkData.fadeValue < 1.0f)
		_chunkData.fadeValue += 3.0f * windowManager->getDeltaTime();
	engine->drawAsset(_asset.assetID, PipelineManager::getPipeline(pipelineType).id);
	if (windowManager->isChunkBordersActive())
		engine->drawAsset(_assetFrame.assetID, PipelineManager::getPipeline(PIPELINE_LINES).id);
}

void Chunk::unload(AEngine * engine)
{
	std::lock_guard<std::mutex> lg(_workerMutex);

	engine->unloadAsset(_asset.assetID);
	engine->unloadAsset(_assetFrame.assetID);
	setState(MESHED);
}

glm::ivec3 Chunk::posToChunkPos(glm::vec3 pos)
{
	glm::vec3 chunkPos;
	chunkPos.x = static_cast<int>(std::floor(static_cast<double>(pos.x) / CHUNK_WIDTH));
	chunkPos.y = static_cast<int>(std::floor(static_cast<double>(pos.y) / CHUNK_HEIGHT));
	chunkPos.z = static_cast<int>(std::floor(static_cast<double>(pos.z) / CHUNK_LENGTH));
	return chunkPos;
}
