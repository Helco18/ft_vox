#include "WindowManager.hpp"
#include "World.hpp"
#include "Logger.hpp"
#include "SimplexNoise.hpp"
#include "Profiler.hpp"
#include "HeightMap.hpp"
#include "TerrainGenerator.hpp"

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
	Profiler p("Chunk::build");
	_isTakenByWorker.store(true);
	std::lock_guard<std::mutex> lg(_workerMutex);

	if (!_world->isLoaded())
			return;

	TerrainGenerator terrainGenerator(this, _world, _chunkLocation);
	terrainGenerator.generateTerrain();

	setState(BUILT);
	setDirty(true);
	dirtyCheck({_northChunk, _southChunk, _westChunk, _eastChunk, _topChunk, _bottomChunk});
	_isTakenByWorker.store(false);
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
	{
		setState(MESHED_EMPTY);
		_chunkData.fadeValue = 1.0f;
	}
	_isTakenByWorker.store(false);
}

void Chunk::uploadAsset(AEngine * engine)
{
	if (!_workerMutex.try_lock())
		return;

	engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_VOXEL).id);
	if (!_assetFrame.isUploaded)
		engine->uploadAsset(_assetFrame, PipelineManager::getPipeline(PIPELINE_LINES).id);
	setState(UPLOADED);
	_workerMutex.unlock();
}

void Chunk::drawAsset(AEngine * engine, PipelineType pipelineType)
{
	Profiler p("Chunk::drawAsset");
	if (!_workerMutex.try_lock())
		return;

	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(engine->getWindow()));
	if (_chunkData.fadeValue < 1.0f && _asset.isUploaded)
		_chunkData.fadeValue += 3.0f * windowManager->getDeltaTime();
	if (_chunkData.fadeValue > 1.0f)
		_chunkData.fadeValue = 1.0f;
	_chunkData.time += windowManager->getDeltaTime();
	_chunkData.maxDistanceRendered = windowManager->getCamera()->getRenderDistance() * World::getRenderDistanceMin();
	engine->drawAsset(_asset.assetID, PipelineManager::getPipeline(pipelineType).id);
	if (windowManager->isChunkBordersActive())
		engine->drawAsset(_assetFrame.assetID, PipelineManager::getPipeline(PIPELINE_LINES).id);
	_workerMutex.unlock();
}

bool Chunk::unloadMesh(AEngine * engine)
{
	if (!_asset.isUploaded)
		return false;
	if (!_workerMutex.try_lock())
		return false;

	engine->unloadAsset(_asset.assetID);
	setState(MESHED);
	setDirty(true);
	_workerMutex.unlock();
	return true;
}

bool Chunk::unload(AEngine * engine)
{
	if (!_asset.isUploaded || !_assetFrame.isUploaded)
		return false;
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
		Logger::log(VOXEL, WARNING, "Requested invalid block change at: "
			+ toString(position.x) + ", "
			+ toString(position.y) + ", "
			+ toString(position.z) + ".");
	}
	{
		std::lock_guard<std::mutex> lg(_workerMutex);
		_blocks[localPos.x][localPos.y][localPos.z] = newType;
	}
	if (localPos.x == CHUNK_WIDTH - 1)
		_world->_dirtyChunks.push_back({_northChunk, glm::vec3(position.x + 1, position.y, position.z)});
	if (localPos.x == 0)
		_world->_dirtyChunks.push_back({_southChunk, glm::vec3(position.x- 1, position.y, position.z)});
	if (localPos.z == CHUNK_LENGTH - 1)
		_world->_dirtyChunks.push_back({_eastChunk, glm::vec3(position.x, position.y, position.z + 1)});
	if (localPos.z == 0)
		_world->_dirtyChunks.push_back({_westChunk, glm::vec3(position.x, position.y, position.z - 1)});
	if (localPos.y == CHUNK_HEIGHT - 1)
		_world->_dirtyChunks.push_back({_topChunk, glm::vec3(position.x, position.y + 1, position.z)});
	if (localPos.y == 0)
		_world->_dirtyChunks.push_back({_bottomChunk, glm::vec3(position.x, position.y - 1, position.z)});
	_world->_dirtyChunks.push_back({this, glm::vec3(position.x, position.y, position.z)});
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
