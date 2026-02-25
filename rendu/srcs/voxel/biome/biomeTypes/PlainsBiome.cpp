#include "PlainsBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double PlainsBiome::computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const
{
	return _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 5.0 + _terrainHeightOffset;
}

uint8_t PlainsBiome::fillWorld(int, int, int height, int worldY, double) const
{
	if (worldY >= -3 && worldY <= -1)
		return BlockType::SAND;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
		return BlockType::DIRT;
}

uint8_t PlainsBiome::splitSkyFromSea(int worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t PlainsBiome::paintSurface(HeightMap &, int, int, int, int worldY, int, double) const
{
	return worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
