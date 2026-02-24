#include "TundraBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

int	TundraBiome::computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const
{
	return static_cast<int>(std::floor(_biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 5) + _terrainHeightOffset);
}

uint8_t TundraBiome::fillWorld(int height, double worldY, float slope) const
{
	if (worldY >= -3 && worldY <= -1)
		return BlockType::SAND;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
	{
		if (slope > 0.05f)
			return BlockType::STONE;
		else
			return BlockType::DIRT;
	}
}

uint8_t TundraBiome::splitSkyFromSea(double worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t TundraBiome::paintSurface(double worldY, float slope) const
{
	if (slope > 0.05f)
		return BlockType::STONE;
	else
		return worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
