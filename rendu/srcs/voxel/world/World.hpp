#pragma once

#include <unordered_map>
#include "AEngine.hpp"
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

		Chunk *					getChunkAt(int x, int y, int z) const;
		Chunk *					getChunkAt(const glm::vec3 & location) const;
		Chunk *					getChunkAtChunkLocation(int x, int y, int z) const;
		Chunk *					getChunkAtChunkLocation(const glm::vec3 & location) const;

		void					addChunk(Chunk * chunk);
		void					reloadChunks(AEngine * engine);

		void					generateProcedurally(Camera * camera);
		void					render(AEngine * engine, PipelineType pipelineType);
	private:
		void					_generateVisibleChunks(Camera * camera);
		void					_generateProceduralTerrain();
		void					_generateProceduralMesh();
		void					_drawChunk(AEngine * engine, PipelineType pipelineType);
		void					_generateChunks();

		typedef std::unordered_map<glm::ivec3, Chunk *> ChunkMap;
		typedef std::vector<Chunk *>					VisibleChunks;

		std::string				_name;
		ChunkMap				_chunkMap;
		VisibleChunks			_visibleChunks;
		ThreadPool				_chunkPool;
};
