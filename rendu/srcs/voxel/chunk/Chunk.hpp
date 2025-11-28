#pragma once

#define CHUNK_WIDTH 3
#define CHUNK_HEIGHT 3
#define CHUNK_LENGTH 3

#include <cstdint>
#include <vector>
#include "OBJModel.hpp"

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

		const glm::vec3			getChunkLocation() const { return _chunkLocation; }
		int						getChunkX() const { return _chunkLocation.x; }
		int						getChunkY() const { return _chunkLocation.y; }
		int						getChunkZ() const { return _chunkLocation.z; }
		ChunkState				getState() const { return _state; }

		void					build();
		void					generateMesh();
		void					upload();
		void					unload();
		void					render();
	private:
		glm::vec3				_chunkLocation;
		uint8_t					_blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
		std::vector<Vertex>		_vertices;
		std::vector<uint32_t>	_indices;
		ChunkState				_state;
};
