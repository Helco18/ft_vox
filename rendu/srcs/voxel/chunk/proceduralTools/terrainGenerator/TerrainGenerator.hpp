#pragma once

#include "HeightMap.hpp"
#include "World.hpp"

#define SEA_LEVEL 0

class TerrainGenerator
{
	public:
		TerrainGenerator(Chunk * chunk, World * world, const glm::vec3 & chunkLocation);
		~TerrainGenerator() {};

		void	generateTerrain();

	private:
		void		_computeTerrainHeight(int x, int z, int worldX, int worldZ);
		void		_computeBlock(int x, int y, int z, double worldX, double worldZ);
		void		_paintSurface(int x, int y, int z, float slope, double worldY);
		void		_splitSkyFromSea(int x, int y, int z, double worldY);
		void		_fillWorld(int x, int y, int z, float slope, double worldY);
		void		_addCave(int x, int y, int z, double worldX, double worldY, double worldZ, int height);

		Chunk *		_chunk;
		World *		_world;
		glm::vec3	_chunkLocation;
		HeightMap	_heightMap;
		int			_height;
		double		_worldXOffset;
		double		_worldYOffset;
		double		_worldZOffset;
};
