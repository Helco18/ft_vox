#pragma once

#include <unordered_map>
#include "Chunk.hpp"

#define KNUTH_CONSTANT 0x9e3779b9

struct Vec3Hash
{
	size_t operator()(const glm::ivec3 & v) const noexcept
	{
		return ((KNUTH_CONSTANT + std::hash<int>()(v.x)) * KNUTH_CONSTANT
				 + std::hash<int>()(v.y)) * KNUTH_CONSTANT
				 + std::hash<int>()(v.z);
	}
};

class World
{
	public:
		World(const std::string & name): _name(name) {}
		~World();

		void				load();

		const std::string &	getName() { return _name; }

		Chunk *				getChunk(const glm::vec3 & location);
		Chunk *				getChunk(int x, int y, int z);

		void				addChunk(Chunk * chunk);
		void				reloadChunks();
	private:
		typedef std::unordered_map<glm::ivec3, Chunk *, Vec3Hash> ChunkMap;

		std::string			_name;
		ChunkMap			_chunkMap;
};
