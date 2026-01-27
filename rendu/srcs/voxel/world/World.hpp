#pragma once

#include <unordered_map>
#include "AEngine.hpp"
#include "Camera.hpp"
#include "PipelineManager.hpp"
#include "Chunk.hpp"
#include "ThreadPool.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

class World
{
	public:
		World(const std::string & name): _name(name) {}
		~World();

		void					load();

		const std::string &		getName() { return _name; }

		Chunk *					getChunkAt(int x, int y, int z);
		Chunk *					getChunkAt(const glm::vec3 & location);
		Chunk *					getChunkAtChunkLocation(int x, int y, int z);
		Chunk *					getChunkAtChunkLocation(const glm::vec3 & location);
		bool					isLoaded() const { return _isLoaded.load(); }

		void					setRenderReady(bool ready) { _renderReady.store(ready); };
		void					unloadChunks(AEngine * engine);

		void					render(AEngine * engine, PipelineType pipelineType);
		void					update(Camera * camera);
	private:
		typedef std::unordered_map<glm::ivec3, Chunk *> ChunkMap;
		typedef std::vector<Chunk *>					ChunkVec;

		void					_generateChunks();
		void					_computeRenderDistance(const int renderDistance);
		bool					_isWithinRenderDistance(const glm::vec3 & chunkPos, const glm::vec3 & camPos);
		ChunkVec				_queryChunksInRange(ChunkState minState = NONE);

		std::string				_name;
		ChunkMap				_chunkMap;
		ChunkVec				_visibleChunks;
		ThreadPool				_chunkPool;
		std::mutex				_mapMutex;
		std::mutex				_renderPointMutex;
		glm::vec3				_renderPoint;
		std::atomic_bool		_isLoaded = false;
		std::atomic_bool		_isProceduralRequested = false;
		std::atomic_bool		_renderReady = false;
		std::condition_variable	_cv;
		int						_renderDistanceX;
		int						_renderDistanceY;
		int						_renderDistanceZ;
};
