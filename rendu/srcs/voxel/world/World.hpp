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

		void					reloadChunks(AEngine * engine);

		void					generateProcedurally(Camera * camera);
		void					render(AEngine * engine, PipelineType pipelineType);
	private:
		typedef std::unordered_map<glm::ivec3, Chunk *> ChunkMap;
		typedef std::vector<Chunk *>					VisibleChunks;

		VisibleChunks			_generateVisibleChunks(Camera * camera);
		void					_generateChunks(Camera * camera);
		void					_generateProceduralTerrain(Camera * camera, VisibleChunks & visibleChunks);
		void					_generateProceduralMesh(Camera * camera, VisibleChunks & visibleChunks);
		void					_computeRenderDistance(Camera * camera);
		bool					_isWithinRenderDistance(Chunk * chunk, Camera * camera);

		std::string				_name;
		ChunkMap				_chunkMap;
		VisibleChunks			_visibleChunks;
		ThreadPool				_chunkPool;
		std::mutex				_chunkMutex;
		std::mutex				_visibleMutex;
		std::mutex				_mapMutex;
		std::condition_variable	_chunkCv;
		std::atomic_bool		_isLoaded = false;
		std::atomic_bool		_isProceduralRequested = false;

		Camera *				_camera;
		int						_renderDistanceX;
		int						_renderDistanceY;
		int						_renderDistanceZ;
};
