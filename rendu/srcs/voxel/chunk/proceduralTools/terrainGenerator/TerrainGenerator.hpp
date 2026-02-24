#pragma once

#include "ABiome.hpp"
#include "HeightMap.hpp"
#include "World.hpp"

#define SEA_LEVEL 0

class TerrainGenerator
{
	public:
		TerrainGenerator(Chunk * chunk, World * world, const glm::vec3 & chunkLocation);
		~TerrainGenerator() {};

		void		generateTerrain();

	private:
		int			_computeTerrainHeight(const ABiome & biome, int x, int z, int worldX, int worldZ);
		uint8_t		_computeBlock(const ABiome & biome, float slope, double worldY, int height);
		void		_addCave(int x, int y, int z, double worldX, double worldY, double worldZ, int height);

		Chunk *		_chunk;
		World *		_world;
		glm::vec3	_chunkLocation;
		HeightMap	_heightMap;
		double		_worldXOffset;
		double		_worldYOffset;
		double		_worldZOffset;
};
