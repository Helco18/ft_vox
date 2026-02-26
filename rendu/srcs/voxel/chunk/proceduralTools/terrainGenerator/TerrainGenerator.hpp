#pragma once

#include "ABiome.hpp"
#include "BiomeManager.hpp"
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
		double		_computeTerrainHeight(const BiomePaintingInfo & paintingInfo);
		uint8_t		_computeBlock(const ABiome & biome, BiomePaintingInfo & biomePaintingInfo);
		void		_addCave(int x, int y, int z, int worldX, int worldY, int worldZ, int height);

		Chunk *		_chunk;
		World *		_world;
		glm::vec3	_chunkLocation;
		HeightMap	_heightMap;
		HeightMap	_biomeMap;
		double		_worldXOffset;
		double		_worldYOffset;
		double		_worldZOffset;
};
