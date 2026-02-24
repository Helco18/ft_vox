#include "PlainsBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

int	PlainsBiome::computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const
{
	return static_cast<int>(std::floor(_biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 5) + _terrainHeightOffset);
}

uint8_t PlainsBiome::fillWorld(int height, double worldY, float) const
{
	if (worldY >= -3 && worldY <= -1)
		return BlockType::SAND;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
		return BlockType::DIRT;
}

uint8_t PlainsBiome::splitSkyFromSea(double worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t PlainsBiome::paintSurface(double worldY, float) const
{
	return worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
