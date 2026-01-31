#include "Chunk.hpp"
#include "Profiler.hpp"
#include "WindowManager.hpp"
#include "World.hpp"
#include "Logger.hpp"
#include "SimplexNoise.hpp"
#include "BlockData.hpp"

Chunk::Chunk(int x, int y, int z, World * world): _world(world), _chunkLocation(glm::ivec3(x, y, z)), _state(NONE)
{
	_asset.uniforms = &_chunkData;
}

static void dirtyCheck(std::vector<Chunk *> chunks)
{
	for (Chunk * chunk : chunks)
	{
		if (!chunk)
			continue;
		ChunkState state = chunk->getState();
		if (chunk && state >= BUILT)
			chunk->setDirty(true);
	}
}

bool Chunk::isReadyForMesh()
{
	std::vector<Chunk *> chunks = _computeNeighborChunks();
	for (Chunk * chunk : chunks)
	{
		if (!chunk || chunk->getState() < BUILT)
			return false;
	}
	return true;
}

void Chunk::build()
{
	std::lock_guard<std::mutex> lg(_workerMutex);
	static SimplexNoise<2> noise(42);

	if (!_world->isLoaded())
			return;
	Profiler p("Chunk::build");
	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int z = 0; z < CHUNK_LENGTH; ++z)
		{
			double xd = static_cast<double>(x + _chunkLocation.x * CHUNK_WIDTH) * 0.01;
			double zd = static_cast<double>(z + _chunkLocation.z * CHUNK_LENGTH) * 0.01;
			if (xd < 0)
				xd *= -1;
			if (zd < 0)
				zd *= -1;
			double noiseValue = noise.queryState({xd, zd});
			int height = static_cast<int>(std::floor(noiseValue * 30));
			for (int y = 0; y < CHUNK_HEIGHT; ++y)
			{
				int worldY = (y + _chunkLocation.y * CHUNK_HEIGHT);
				if (worldY == height && worldY >= 0)
					_blocks[x][y][z] = BlockType::GRASS;
				else if (worldY > height)
					_blocks[x][y][z] = (worldY) <= 0 ? BlockType::WATER : BlockType::AIR;
				else
					_blocks[x][y][z] = BlockType::DIRT;
			}
		}
	}
	setState(BUILT);
	setDirty(true);
	dirtyCheck({_northChunk, _southChunk, _westChunk, _eastChunk, _topChunk, _bottomChunk});
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
	_generateGreedyMesh();
	if (_asset.vertices.data && !_asset.indices.empty())
		setState(MESHED);
	else
		setState(MESHED_EMPTY);
}

void Chunk::uploadAsset(AEngine * engine)
{
	if (_workerMutex.try_lock())
		return;

	Profiler p("Chunk::uploadAsset");
	engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_VOXEL).id);
	engine->uploadAsset(_assetFrame, PipelineManager::getPipeline(PIPELINE_LINES).id);
	setState(UPLOADED);
}

void Chunk::drawAsset(AEngine * engine, PipelineType pipelineType)
{
	if (_workerMutex.try_lock())
		return;

	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(engine->getWindow()));
	if (_chunkData.fadeValue < 1.0f && _asset.isUploaded)
		_chunkData.fadeValue += 3.0f * windowManager->getDeltaTime();
	if (_chunkData.fadeValue > 1.0f)
		_chunkData.fadeValue = 1.0f;
	engine->drawAsset(_asset.assetID, PipelineManager::getPipeline(pipelineType).id);
	if (windowManager->isChunkBordersActive())
		engine->drawAsset(_assetFrame.assetID, PipelineManager::getPipeline(PIPELINE_LINES).id);
}

void Chunk::unload(AEngine * engine)
{
	if (_workerMutex.try_lock())
		return;
	engine->unloadAsset(_asset.assetID);
	engine->unloadAsset(_assetFrame.assetID);
	setState(MESHED);
	setDirty(true);
}

glm::ivec3 Chunk::posToChunkPos(glm::vec3 pos)
{
	glm::vec3 chunkPos;
	chunkPos.x = static_cast<int>(std::floor(static_cast<double>(pos.x) / CHUNK_WIDTH));
	chunkPos.y = static_cast<int>(std::floor(static_cast<double>(pos.y) / CHUNK_HEIGHT));
	chunkPos.z = static_cast<int>(std::floor(static_cast<double>(pos.z) / CHUNK_LENGTH));
	return chunkPos;
}

std::vector<Chunk *> Chunk::_computeNeighborChunks()
{
	_northChunk = _world->getChunkAtChunkLocation(_chunkLocation.x + 1, _chunkLocation.y, _chunkLocation.z);
	_southChunk = _world->getChunkAtChunkLocation(_chunkLocation.x - 1, _chunkLocation.y, _chunkLocation.z);
	_eastChunk = _world->getChunkAtChunkLocation(_chunkLocation.x, _chunkLocation.y, _chunkLocation.z + 1);
	_westChunk = _world->getChunkAtChunkLocation(_chunkLocation.x, _chunkLocation.y, _chunkLocation.z - 1);
	_topChunk = _world->getChunkAtChunkLocation(_chunkLocation.x, _chunkLocation.y + 1, _chunkLocation.z);
	_bottomChunk = _world->getChunkAtChunkLocation(_chunkLocation.x, _chunkLocation.y - 1, _chunkLocation.z);
	return {_northChunk, _southChunk, _eastChunk, _westChunk, _topChunk, _bottomChunk};
}
