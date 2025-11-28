#pragma once

#define CHUNK_WIDTH 2
#define CHUNK_HEIGHT 2
#define CHUNK_LENGTH 2

#include <cstdint>
#include <vector>
#include "OBJModel.hpp"
#include "AEngine.hpp"

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
		Chunk(int x = 0, int y = 0, int z = 0): _chunkLocation(glm::vec3(x, y, z)), _state(NONE) {}
		Chunk(const glm::vec3 & chunkLocation): _chunkLocation(chunkLocation), _state(NONE) {}
		~Chunk() {};

		const glm::vec3 &		getChunkLocation() const { return _chunkLocation; }
		int						getChunkX() const { return _chunkLocation.x; }
		int						getChunkY() const { return _chunkLocation.y; }
		int						getChunkZ() const { return _chunkLocation.z; }
		ChunkState				getState() const { return _state; }
		Asset &					getAsset() { return _asset; }

		void					build();
		void					generateMesh();
		void					upload(AEngine * engine);
		void					unload();
	private:
		glm::vec3				_chunkLocation;
		uint8_t					_blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
		Asset					_asset;
		ChunkState				_state;
};
