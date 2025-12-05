#pragma once

#define CHUNK_WIDTH 10
#define CHUNK_HEIGHT 10
#define CHUNK_LENGTH 10

#include <cstdint>
#include <vector>
#include "OBJModel.hpp"
#include "AEngine.hpp"

class World;

enum ChunkState
{
	NONE,
	BUILT,
	MESHED,
	UPLOADED
};

class Chunk
{
	public:
		Chunk(int x = 0, int y = 0, int z = 0, World * world = nullptr): _world(world), _chunkLocation(glm::vec3(x, y, z)), _state(NONE) {}
		Chunk(const glm::vec3 & chunkLocation, World * world): _world(world), _chunkLocation(chunkLocation), _state(NONE){}
		~Chunk() {};

		const glm::vec3 &		getChunkLocation() const { return _chunkLocation; }
		int						getChunkX() const { return _chunkLocation.x; }
		int						getChunkY() const { return _chunkLocation.y; }
		int						getChunkZ() const { return _chunkLocation.z; }
		ChunkState				getState() const { return _state; }
		Asset &					getAsset() { return _asset; }
		uint8_t					getBlock(int x, int y, int z) { return _blocks[x][y][z]; }

		void					build();
		void					generateMesh();
		void					uploadAsset(AEngine * engine);
		void					unload();
	private:
		World *					_world;
		glm::vec3				_chunkLocation;
		uint8_t					_blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
		Asset					_asset;
		ChunkState				_state;

		std::vector<uint8_t>	_getNeighboringBlocks(int x, int y, int z);
		bool					_blockIsVisible(int x, int y, int z);
};
