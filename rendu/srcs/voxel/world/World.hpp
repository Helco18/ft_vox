#pragma once

#include <unordered_map>
#include "AEngine.hpp"
#include "Chunk.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

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

		void				render(AEngine * engine);
	private:
		typedef std::unordered_map<glm::ivec3, Chunk *> ChunkMap;

		std::string			_name;
		ChunkMap			_chunkMap;
};
