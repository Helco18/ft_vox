#include "MountainsBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

int	MountainsBiome::computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const
{
	double noiseValue = _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)});
	if (noiseValue > -0.5 && noiseValue <= 0.5)
		noiseValue = noiseValue * 110 + 45;
	else if (noiseValue > 0.5)
		noiseValue = noiseValue * 200 + 0;
	else
		noiseValue = noiseValue * 40 + 10;
	return static_cast<int>(std::floor(noiseValue) + _terrainHeightOffset);
}

uint8_t MountainsBiome::fillWorld(int height, double worldY, float slope) const
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

uint8_t MountainsBiome::splitSkyFromSea(double worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t MountainsBiome::paintSurface(double worldY, float slope) const
{
	if (slope > 0.05f)
		return BlockType::STONE;
	else
		return worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
