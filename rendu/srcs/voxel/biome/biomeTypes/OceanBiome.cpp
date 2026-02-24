#include "OceanBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

int	OceanBiome::computeBiomeHeight(HeightMap & heightMap, int x, int z, int, int) const
{
	return static_cast<int>(std::floor(heightMap.getHeight(x, z) * 10) + _terrainHeightOffset);
}

uint8_t OceanBiome::fillWorld(int height, double worldY, float) const
{
	if (worldY >= -3 && worldY <= -1)
		return BlockType::SAND;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
		return BlockType::DIRT;
}

uint8_t OceanBiome::splitSkyFromSea(double worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t OceanBiome::paintSurface(double worldY, float) const
{
	return worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
