#pragma once

#include <memory>
#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 32
#define CHUNK_LENGTH 32

#include <mutex>
#include "OBJModel.hpp"
#include "AEngine.hpp"
#include "BlockData.hpp"

class World;

enum ChunkState
{
	IDLE,
	BUILDING,
	BUILT,
	MESHING,
	MESHED,
	MESHED_EMPTY,
	UPLOADED
};

enum FaceDirection
{
	FORWARD,
	BACKWARD
};

struct ChunkVertex
{
	glm::vec3	position;
	glm::vec3	normal;
	float		alpha;
	glm::vec2	texCoord;
	glm::vec2	uvMin;
	glm::vec2	uvMax;
	glm::vec2	uvRepeat;
};

struct ChunkAsset
{
	std::vector<ChunkVertex>	vertices;
	std::vector<uint32_t>		indices;
};

struct alignas(16) ChunkData
{
	float		fadeValue;
	float		maxDistanceRendered;
    float		_pad[2];
	glm::mat4	model;
};

class Chunk
{
	friend class TerrainGenerator;

	public:
		Chunk(int x = 0, int y = 0, int z = 0, World * world = nullptr);
		Chunk(const glm::ivec3 & chunkLocation, World * world = nullptr): Chunk(chunkLocation.x, chunkLocation.y, chunkLocation.z, world) {}
		~Chunk() {};

		const glm::ivec3 &						getChunkLocation() const { return _chunkLocation; }
		int										getChunkX() const { return _chunkLocation.x; }
		int										getChunkY() const { return _chunkLocation.y; }
		int										getChunkZ() const { return _chunkLocation.z; }
		ChunkState								getState() const { return _state.load(); }
		Asset &									getAsset() { return _asset; }
		uint8_t									getBlock(int x, int y, int z) { return _blocks[x][y][z].load(std::memory_order_relaxed); }
		inline float							getDistance(glm::vec3 pos) const { return(glm::distance((glm::vec3)locToChunkLoc(pos), (glm::vec3)_chunkLocation)); };
		const glm::vec3 &						getMin() const { return _min; };
		const glm::vec3 &						getMax() const { return _max; };
		bool									isDirty() const { return _isDirty.load(); }
		bool									isTakenByWorker() const { return _isTakenByWorker.load(); }
		bool									isMarkedForDeletion() const { return _deleted.load(); }

		void									setState(ChunkState state) { _state.store(state); }
		void									setDirty(bool dirty) { _isDirty.store(dirty); }
		void									setBlockAt(const glm::vec3 & position, BlockType newType);
		void									markDeleted() { _deleted.store(true); };
		
		void									build();
		void									generateMesh();
		void									uploadAsset(AEngine * engine);
		void									drawAsset(AEngine * engine, PipelineType pipelineType);
		bool									unloadMesh(AEngine * engine);
		bool									unload(AEngine * engine);

		static glm::ivec3						posToChunkPos(const glm::vec3 & pos);
		static glm::ivec3						locToChunkLoc(const glm::vec3 & loc);
		bool									isReadyForMesh();

		void									updateMesh(const glm::vec3 & pos);

	private:
		World *									_world;
		glm::ivec3								_chunkLocation;
		std::atomic<uint8_t>					_blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];

		std::array<std::vector<ChunkAsset>, 3>	_chunkOpaqueAsset;
		std::array<std::vector<ChunkAsset>, 3>	_chunkTransparencyAsset;
		ChunkAsset								_chunkFinalAsset;

		Asset									_assetFrame;
		glm::mat4								_frameModel;
		Asset									_asset;

		std::vector<ChunkVertex>				_vertices;
		std::vector<glm::vec3>					_linesPos;
		std::atomic<ChunkState>					_state;
		std::mutex								_workerMutex;
		ChunkData								_chunkData { 0.0f,  0.0f, {0.0f, 0.0f}, glm::mat4(1.0f) };
		std::atomic_bool						_isDirty = false;
		std::atomic_bool						_isTakenByWorker = false;
		std::atomic_bool						_deleted = false;

		std::weak_ptr<Chunk> 					_self;
		std::weak_ptr<Chunk> 					_northChunk;
		std::weak_ptr<Chunk> 					_southChunk;
		std::weak_ptr<Chunk> 					_eastChunk;
		std::weak_ptr<Chunk> 					_westChunk;
		std::weak_ptr<Chunk> 					_topChunk;
		std::weak_ptr<Chunk> 					_bottomChunk;

		glm::vec3								_min;
		glm::vec3								_max;

		void									_generateGreedyMesh();
		void									_buildAsset();
		void									_processFace(int u, int v, std::vector<std::array<bool,2>> & processed, FaceDirection faceDir, int axis, int sliceIndex, int uMax, int vMax);
		ChunkAsset								_generateQuadMesh(float width, float height, float depth, int face);
		void									_emitBlocksFace(const glm::ivec3 & pos, int countBlockWidth, int countBlockHeight, int face, int axis, int sliceIndex);
		void									_generateSliceMeshing(int axis, int sliceIndex);
		uint8_t									_getNeighborBlock(const glm::ivec3 & pos, const glm::ivec3 & normal);
		glm::ivec3								_sliceToWorld(int axis, int sliceIndex, int u, int v);
		void									_generateFrameMesh();
		std::array<std::weak_ptr<Chunk>, 6> 	_computeNeighborChunks();
		glm::vec3								_computeQuadSize(const glm::ivec3 & pos, int face);
};