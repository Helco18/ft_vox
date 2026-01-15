#pragma once

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_LENGTH 16

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

class Chunk
{
	public:
		Chunk(int x = 0, int y = 0, int z = 0, World * world = nullptr): _world(world), _chunkLocation(glm::ivec3(x, y, z)), _state(NONE) {}
		Chunk(const glm::ivec3 & chunkLocation, World * world): _world(world), _chunkLocation(chunkLocation), _state(NONE){}
		~Chunk() {};

		const glm::ivec3 &		getChunkLocation() const { return _chunkLocation; }
		int						getChunkX() const { return _chunkLocation.x; }
		int						getChunkY() const { return _chunkLocation.y; }
		int						getChunkZ() const { return _chunkLocation.z; }
		ChunkState				getState();
		Asset &					getAsset() { return _asset; }
		uint8_t					getBlock(int x, int y, int z) { return _blocks[x][y][z]; }

		void					setState(ChunkState state);

		void					build();
		void					generateMesh();
		void					uploadAsset(AEngine * engine);
		void					unload(AEngine * engine);
	private:
		World *					_world;
		glm::ivec3				_chunkLocation;
		uint8_t					_blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
		ChunkAsset				_chunkAsset;
		Asset					_asset;
		std::vector<Vertex>		_vertices;
		ChunkState				_state;
		std::mutex				_stateMutex;
		std::mutex				_workerMutex;

		void					_generateGreedyMesh();
		void					_processFace(int u, int v, std::vector<std::vector<std::array<bool,2>>> & processed, FaceDirection faceDir, int axis, int sliceIndex, int uMax, int vMax);
		ChunkAsset				_generateQuadMesh(float width, float height, float depth, int face);
		void					_emitBlocksFace(const glm::ivec3 & pos, int countBlockWidth, int countBlockHeight, int face);
		void					_generateSliceMeshing(int axis, int sliceIndex);
		uint8_t					_getNeighborBlock(const glm::ivec3 & pos, const glm::ivec3 & normal);
		glm::ivec3				_sliceToWorld(int axis, int sliceIndex, int u, int v);
};
