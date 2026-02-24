#include "DesertBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

int	DesertBiome::computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const
{
	return static_cast<int>(std::floor(_biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 8) + _terrainHeightOffset);
}

uint8_t DesertBiome::fillWorld(int height, double worldY, float) const
{
	if (worldY >= -3)
		return BlockType::SANDSTONE;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
		return BlockType::SAND;
}

uint8_t DesertBiome::splitSkyFromSea(double worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t DesertBiome::paintSurface(double, float) const
{
	return BlockType::SAND;
}
