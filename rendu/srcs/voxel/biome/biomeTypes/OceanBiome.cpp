#include "OceanBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double	OceanBiome::computeBiomeHeight(HeightMap & heightMap, int x, int z, int, int) const
{
	return heightMap.getHeight(x, z) * 10.0 + _terrainHeightOffset;
}

uint8_t OceanBiome::fillWorld(int, int, int height, int worldY, double) const
{
	if (worldY >= -3 && worldY <= -1)
		return BlockType::SAND;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
		return BlockType::DIRT;
}

uint8_t OceanBiome::splitSkyFromSea(int worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t OceanBiome::paintSurface(HeightMap &, int, int, int, int worldY, int, double) const
{
	return worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
