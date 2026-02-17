#include "Chunk.hpp"
#include "WindowManager.hpp"
#include "World.hpp"
#include "Logger.hpp"
#include "SimplexNoise.hpp"

Chunk::Chunk(int x, int y, int z, World * world): _world(world), _chunkLocation(glm::ivec3(x, y, z)), _state(IDLE)
{
	_min = _chunkLocation;
	_min *= glm::vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_LENGTH);
	_max = _min + glm::vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_LENGTH);
	_asset.uniforms = &_chunkData;
	_asset.vertices.data = nullptr;
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
	std::lock_guard<std::mutex> lg(_workerMutex);
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
	_isTakenByWorker.store(true);
	std::lock_guard<std::mutex> lg(_workerMutex);

	if (!_world->isLoaded())
			return;
	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		double worldX = static_cast<double>(x + _chunkLocation.x * CHUNK_WIDTH);
		for (int z = 0; z < CHUNK_LENGTH; ++z)
		{
			double worldZ = static_cast<double>(z + _chunkLocation.z * CHUNK_LENGTH);
			int height;
			if (_chunkLocation.y < -2)
				height = 0;
			else if (_chunkLocation.y > 2)
				height = 0;
			else
			{
				double noiseValue = _world->getNoise().queryState({worldX, worldZ});
				height = static_cast<int>(std::floor(noiseValue * 30));
			}
			for (int y = 0; y < CHUNK_HEIGHT; ++y)
			{
				int worldY = (y + _chunkLocation.y * CHUNK_HEIGHT);
				if (worldY == height && worldY >= 0)
					_blocks[x][y][z] = worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
				else if (worldY > height)
					_blocks[x][y][z] = (worldY) <= 0 ? BlockType::WATER : BlockType::AIR;
				else
				{
					if (worldY >= -3 && worldY <= -1)
						_blocks[x][y][z] = BlockType::SAND;
					else if (worldY <= height - 2 - (height % 2))
						_blocks[x][y][z] = BlockType::STONE;
					else
						_blocks[x][y][z] = BlockType::DIRT;
				}
			}
		}
	}
	setState(BUILT);
	setDirty(true);
	dirtyCheck({_northChunk, _southChunk, _westChunk, _eastChunk, _topChunk, _bottomChunk});
	_isTakenByWorker.store(false);
}

float Chunk::getDistance(glm::vec3 pos) const
{
	return(glm::distance((glm::vec3)locToChunkLoc(pos), (glm::vec3)_chunkLocation));
}

void Chunk::generateMesh()
{
	_isTakenByWorker.store(true);
	std::lock_guard<std::mutex> lg(_workerMutex);

	if (!_world->isLoaded())
		return;
	_generateGreedyMesh();
	if (_asset.vertices.data && !_asset.indices.empty())
		setState(MESHED);
	else
		setState(MESHED_EMPTY);
	_isTakenByWorker.store(false);
}

void Chunk::uploadAsset(AEngine * engine)
{
	if (!_workerMutex.try_lock())
		return;

	engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_VOXEL).id);
	engine->uploadAsset(_assetFrame, PipelineManager::getPipeline(PIPELINE_LINES).id);
	setState(UPLOADED);
	_workerMutex.unlock();
}

void Chunk::drawAsset(AEngine * engine, PipelineType pipelineType)
{
	if (!_workerMutex.try_lock())
		return;

	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(engine->getWindow()));
	if (_chunkData.fadeValue < 1.0f && _asset.isUploaded)
		_chunkData.fadeValue += 3.0f * windowManager->getDeltaTime();
	if (_chunkData.fadeValue > 1.0f)
		_chunkData.fadeValue = 1.0f;
	engine->drawAsset(_asset.assetID, PipelineManager::getPipeline(pipelineType).id);
	if (windowManager->isChunkBordersActive())
		engine->drawAsset(_assetFrame.assetID, PipelineManager::getPipeline(PIPELINE_LINES).id);
	_workerMutex.unlock();
}

bool Chunk::unload(AEngine * engine)
{
	if (!_workerMutex.try_lock())
		return false;

	engine->unloadAsset(_asset.assetID);
	engine->unloadAsset(_assetFrame.assetID);
	setState(MESHED);
	setDirty(true);
	_workerMutex.unlock();
	return true;
}

glm::ivec3 Chunk::posToChunkPos(const glm::vec3 & loc)
{
	glm::vec3	chunkPos;
	glm::ivec3	block = floor(loc);

	chunkPos.x = ((block.x % CHUNK_WIDTH) + CHUNK_WIDTH) % CHUNK_WIDTH;
	chunkPos.y = ((block.y % CHUNK_HEIGHT) + CHUNK_HEIGHT) % CHUNK_HEIGHT;
	chunkPos.z = ((block.z % CHUNK_LENGTH) + CHUNK_LENGTH) % CHUNK_LENGTH;
	return chunkPos;
}

glm::ivec3 Chunk::locToChunkLoc(const glm::vec3 & loc)
{
	glm::vec3 chunkLoc;
	chunkLoc.x = static_cast<int>(std::floor(static_cast<double>(loc.x) / CHUNK_WIDTH));
	chunkLoc.y = static_cast<int>(std::floor(static_cast<double>(loc.y) / CHUNK_HEIGHT));
	chunkLoc.z = static_cast<int>(std::floor(static_cast<double>(loc.z) / CHUNK_LENGTH));
	return chunkLoc;
}

void Chunk::setBlockAt(const glm::vec3 & position, BlockType newType)
{
	glm::ivec3 localPos = posToChunkPos(position);
	if (localPos.x >= CHUNK_WIDTH || localPos.x < 0
		|| localPos.y >= CHUNK_HEIGHT || localPos.y < 0
		|| localPos.z >= CHUNK_LENGTH || localPos.z < 0)
	{
		Logger::log(VOXEL, WARNING, "Requested invalid block change at: "\
			+ toString(position.x) + ", "
			+ toString(position.y) + ", "
			+ toString(position.z) + ".");
	}
	{
		std::lock_guard<std::mutex> lg(_workerMutex);
		_blocks[localPos.x][localPos.y][localPos.z] = newType;
	}
	_world->_dirtyChunks.push_back(this);
	_world->_dirtyChunks.push_back(_northChunk);
	_world->_dirtyChunks.push_back(_southChunk);
	_world->_dirtyChunks.push_back(_eastChunk);
	_world->_dirtyChunks.push_back(_westChunk);
	_world->_dirtyChunks.push_back(_topChunk);
	_world->_dirtyChunks.push_back(_bottomChunk);
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
