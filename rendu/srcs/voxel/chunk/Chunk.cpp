#include "Chunk.hpp"
#include "Profiler.hpp"
#include "WindowManager.hpp"
#include "utils.hpp"
#include "World.hpp"
#include "Logger.hpp"
#include "TextureAtlas.hpp"
#include <iostream>
#include "SimplexNoise.hpp"

Chunk::Chunk(int x, int y, int z, World * world): _world(world), _chunkLocation(glm::ivec3(x, y, z)), _state(NONE)
{
	_asset.uniforms = &_chunkData;
}

void Chunk::build()
{
	std::lock_guard<std::mutex> lg(_workerMutex);
	SimplexNoise<2> noise(42);

	if (!_world->isLoaded())
			return;
	Profiler p("Chunk::build");
	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int y = 0; y < CHUNK_HEIGHT; ++y)
		{
			for (int z = 0; z < CHUNK_LENGTH; ++z)
			{
				if (noise.queryState({static_cast<double>(x + _chunkLocation.x * CHUNK_WIDTH), static_cast<double>(y + _chunkLocation.y * CHUNK_HEIGHT), static_cast<double>(z + _chunkLocation.z * CHUNK_LENGTH)}))
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


float Chunk::getDistance(glm::vec3 pos) const
{
	return(glm::distance((glm::vec3)posToChunkPos(pos), (glm::vec3)_chunkLocation));
}

void Chunk::generateMesh()
{
	std::lock_guard<std::mutex> lg(_workerMutex);

	if (!_world->isLoaded())
		return;
	Profiler p("Chunk::generateMesh");
	_computeNeighborChunks();
	_generateGreedyMesh();
	if (_asset.vertices.data && !_asset.indices.empty())
		setState(MESHED);
	else
		setState(MESHED_EMPTY);
}

void Chunk::uploadAsset(AEngine * engine)
{
	Profiler p("Chunk::uploadAsset");
	engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_VOXEL).id);
	engine->uploadAsset(_assetFrame, PipelineManager::getPipeline(PIPELINE_LINES).id);
	setState(UPLOADED);
}

void Chunk::drawAsset(AEngine * engine, PipelineType pipelineType)
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(engine->getWindow()));
	if (_chunkData.fadeValue < 1.0f && _asset.isUploaded)
		_chunkData.fadeValue += 3.0f * windowManager->getDeltaTime();
	engine->drawAsset(_asset.assetID, PipelineManager::getPipeline(pipelineType).id);
	if (windowManager->isChunkBordersActive())
		engine->drawAsset(_assetFrame.assetID, PipelineManager::getPipeline(PIPELINE_LINES).id);
}

void Chunk::unload(AEngine * engine)
{
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

void Chunk::_computeNeighborChunks()
{
	_northChunk = _world->getChunkAtChunkLocation(_chunkLocation.x + 1, _chunkLocation.y, _chunkLocation.z);
	_southChunk = _world->getChunkAtChunkLocation(_chunkLocation.x - 1, _chunkLocation.y, _chunkLocation.z);
	_eastChunk = _world->getChunkAtChunkLocation(_chunkLocation.x, _chunkLocation.y, _chunkLocation.z + 1);
	_westChunk = _world->getChunkAtChunkLocation(_chunkLocation.x, _chunkLocation.y, _chunkLocation.z - 1);
	_topChunk = _world->getChunkAtChunkLocation(_chunkLocation.x, _chunkLocation.y + 1, _chunkLocation.z);
	_bottomChunk = _world->getChunkAtChunkLocation(_chunkLocation.x, _chunkLocation.y - 1, _chunkLocation.z);
}
