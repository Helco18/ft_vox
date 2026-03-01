#pragma once

#include <unordered_map>
#include "AEngine.hpp"
#include "Camera.hpp"
#include "PipelineManager.hpp"
#include "Chunk.hpp"
#include "SimplexNoise.hpp"
#include "ThreadPool.hpp"
#include "BlockData.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#define RENDER_DISTANCE_BORDER 1
#define MAX_UPLOAD_PER_FRAME 128
#define CHUNK_DELETION_DISTANCE 4

struct TargetedBlock
{
	BlockFace 	face;
	glm::vec3 	pos;
	BlockType	type;
};

struct RayState
{
	glm::vec3 ray;
	glm::vec3 delta;
	glm::vec3 o;
};

class World
{
	public:
		friend class Chunk;

		World(const std::string & name, uint32_t seed): _seed(seed),
			_terrainNoise(SimplexNoise<2>(seed, 0.005f, 100000.0f)),
			_temperatureNoise(SimplexNoise<2>(seed, 0.0005f, 100000.0f)),
			_heightNoise(SimplexNoise<2>(seed, 0.0001f, 100000.0f)),
			_caveNoise(SimplexNoise<3>(seed, 0.005f, 100000.0f)),
			_name(name) {}
		~World();

		void										load();

		const std::string &							getName() { return _name; }

		std::shared_ptr<Chunk>						getChunkAt(int x, int y, int z);
		std::shared_ptr<Chunk>						getChunkAt(const glm::vec3 & location);
		std::shared_ptr<Chunk>						getChunkAtChunkLocation(int x, int y, int z);
		std::shared_ptr<Chunk>						getChunkAtChunkLocation(const glm::vec3 & location);
		const SimplexNoise<2> &						getTerrainNoise() const { return _terrainNoise; }
		const SimplexNoise<2> &						getTemperatureNoise() const { return _temperatureNoise; }
		const SimplexNoise<2> &						getHeightNoise() const { return _heightNoise; }
		const SimplexNoise<3> &						getNoiseCave() const { return _caveNoise; }
		bool										isLoaded() const { return _isLoaded.load(); }
		bool										isLocked() const { return _isLocked.load(); }

		void										lockGeneration(bool locked) { _isLocked.store(locked); }
		void										unloadChunks(AEngine * engine);
		void										render(AEngine * engine, PipelineType pipelineType, Camera * camera);
		void										update(AEngine * engine, Camera * camera);

		static int									getRenderDistanceMin();
 		TargetedBlock								rayCast(const glm::vec3 & pos, const glm::vec3 & dir, float maxDistance);

		void										requestProcedural() { _isProceduralRequested.store(true); _cv.notify_one(); } // DEBUG ONLY!

	private:
		typedef std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>	ChunkMap;
		typedef std::vector<std::shared_ptr<Chunk>>						ChunkVec;

		void										_generateChunks();
		void										_computeRenderDistance(const int renderDistance);
		bool										_isWithinRenderDistance(const glm::vec3 & chunkPos, const glm::vec3 & camPos);
		ChunkVec									_queryChunksInRange();
		void										_checkForChunkDeletion(AEngine * engine, Camera * camera);
		bool										_chunkIsFrustum(const Plane * planes, Chunk * chunk);

		TargetedBlock								_processRay(const glm::vec3 & pos, RayState & state, float maxDistance);

		uint32_t									_seed;
		SimplexNoise<2>								_terrainNoise;
		SimplexNoise<2>								_temperatureNoise;
		SimplexNoise<2>								_heightNoise;
		SimplexNoise<3>								_caveNoise;
		std::string									_name;
		ChunkMap									_chunkMap;
		ChunkVec									_visibleChunks;
		ChunkVec									_nextVisibleChunks;
		ChunkVec									_uploadedChunks;
		std::vector<std::pair<std::weak_ptr<Chunk>, glm::vec3>>	_dirtyChunks;
		std::atomic_bool							_readyToSwap = false;
		ThreadPool									_chunkPool;
		std::mutex									_mapMutex;
		std::mutex									_visibleChunksMutex;
		glm::vec3									_renderPoint;
		std::atomic_bool							_isLoaded = false;
		std::atomic_bool							_isProceduralRequested = false;
		std::atomic_bool							_isLocked = false;
		std::condition_variable						_cv;
		glm::ivec3									_renderDistance = glm::ivec3(0.0);
};
