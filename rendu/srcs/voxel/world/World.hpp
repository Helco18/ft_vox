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
		bool					isLocked() const { return _isLocked.load(); }

		void					lockGeneration(bool locked) { _isLocked.store(locked); }

		void					unloadChunks(AEngine * engine);

		void					render(AEngine * engine, PipelineType pipelineType);
		void					update(Camera * camera);

		void					requestProcedural() { _isProceduralRequested.store(true); _cv.notify_one(); } // DEBUG ONLY!
	private:
		typedef std::unordered_map<glm::ivec3, Chunk *> ChunkMap;
		typedef std::vector<Chunk *>					ChunkVec;

		void					_generateChunks();
		void					_computeRenderDistance(const int renderDistance);
		bool					_isWithinRenderDistance(const glm::vec3 & chunkPos, const glm::vec3 & camPos);
		ChunkVec				_queryChunksInRange();

		std::string				_name;
		ChunkMap				_chunkMap;
		ChunkVec				_visibleChunks;
		ThreadPool				_chunkPool;
		std::mutex				_mapMutex;
		std::mutex				_renderPointMutex;
		glm::vec3				_renderPoint;
		std::atomic_bool		_isLoaded = false;
		std::atomic_bool		_isProceduralRequested = false;
		std::condition_variable	_cv;
		std::atomic_bool		_isLocked = false;
		glm::ivec3				_renderDistance = glm::ivec3(0.0);
};
