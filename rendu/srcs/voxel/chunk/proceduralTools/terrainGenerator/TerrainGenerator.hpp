#pragma once

#include "HeightMap.hpp"
#include "World.hpp"

class TerrainGenerator
{
	public:
		TerrainGenerator(World * world, const glm::vec3 & chunkLocation);
		~TerrainGenerator() {};

		void	generateTerrain(Chunk * chunk);

	private:
		void	_addCave(Chunk * chunk, double worldX, double worldY, double worldZ, int x, int y, int z, int height);

		World *		_world;
		glm::vec3	_chunkLocation;
		HeightMap	_heightMap;
		double		_worldXOffset;
		double		_worldYOffset;
		double		_worldZOffset;
};
