#pragma once

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_LENGTH 16

#include <atomic>
#include <cstdint>
#include <vector>
#include <mutex>
#include <fstream>
#include "OBJModel.hpp"
#include "AEngine.hpp"
#include "BlockData.hpp"

class World;

enum ChunkState
{
	NONE,
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

struct ChunkAsset
{
	std::vector<Vertex>		vertices;
	std::vector<uint32_t>	indices;
};

struct ChunkData
{
	float	fadeValue;
};

class Chunk
{
	public:
		Chunk(int x = 0, int y = 0, int z = 0, World * world = nullptr);
		Chunk(const glm::ivec3 & chunkLocation, World * world = nullptr): Chunk(chunkLocation.x, chunkLocation.y, chunkLocation.z, world) {}
		~Chunk() {};

		const glm::ivec3 &		getChunkLocation() const { return _chunkLocation; }
		int						getChunkX() const { return _chunkLocation.x; }
		int						getChunkY() const { return _chunkLocation.y; }
		int						getChunkZ() const { return _chunkLocation.z; }
		ChunkState				getState() const { return _state.load(); }
		Asset &					getAsset() { return _asset; }
		uint8_t					getBlock(int x, int y, int z) { return _blocks[x][y][z].load(); }
		float					getDistance(glm::vec3 pos) const;
		bool					isDirty() const { return _isDirty.load(); }

		void					setState(ChunkState state) { _state.store(state); }
		void					setDirty(bool dirty) { _isDirty.store(dirty); }

		void					build();
		void					generateMesh();
		void					uploadAsset(AEngine * engine);
		void					drawAsset(AEngine * engine, PipelineType pipelineType);
		void					unload(AEngine * engine);

		static glm::ivec3		posToChunkPos(glm::vec3 pos);

	private:
		World *					_world;
		glm::ivec3				_chunkLocation;
		std::atomic<uint8_t>	_blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
		ChunkAsset				_chunkOpaqueAsset;
		ChunkAsset				_chunkTransparencyAsset;
		ChunkAsset				_chunkFinalAsset;
		Asset					_asset;
		Asset					_assetFrame;
		std::vector<Vertex>		_vertices;
		std::vector<glm::vec3>	_linesPos;
		std::atomic<ChunkState>	_state;
		std::mutex				_workerMutex;
		ChunkData				_chunkData { 0.0f };
		std::atomic_bool		_isDirty = false;

		Chunk * 				_northChunk = nullptr;
		Chunk * 				_southChunk = nullptr;
		Chunk * 				_eastChunk = nullptr;
		Chunk * 				_westChunk = nullptr;
		Chunk * 				_topChunk = nullptr;
		Chunk * 				_bottomChunk = nullptr;

		void					_generateGreedyMesh();
		void					_processFace(int u, int v, std::vector<std::vector<std::array<bool,2>>> & processed, FaceDirection faceDir, int axis, int sliceIndex, int uMax, int vMax);
		ChunkAsset				_generateQuadMesh(float width, float height, float depth, int face);
		void					_emitBlocksFace(const glm::ivec3 & pos, int countBlockWidth, int countBlockHeight, int face);
		void					_generateSliceMeshing(int axis, int sliceIndex);
		uint8_t					_getNeighborBlock(const glm::ivec3 & pos, const glm::ivec3 & normal);
		glm::ivec3				_sliceToWorld(int axis, int sliceIndex, int u, int v);
		void					_generateFrameMesh();
		void					_computeNeighborChunks();
};
