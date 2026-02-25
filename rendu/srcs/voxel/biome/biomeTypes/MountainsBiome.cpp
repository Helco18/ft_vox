#include "MountainsBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double MountainsBiome::computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const
{
	double noiseValue = _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)});
	if (noiseValue > -0.5 && noiseValue <= 0.5)
		noiseValue = noiseValue * 110 + 45;
	else if (noiseValue > 0.5)
		noiseValue = noiseValue * 200 + 0;
	else
		noiseValue = noiseValue * 40 + 10;
	return noiseValue + _terrainHeightOffset;
}

uint8_t MountainsBiome::fillWorld(int, int, int height, int worldY, double slope) const
{
	if (slope < 10.0 - (15 - worldY * 0.05) && worldY > height - 2 - (height % 2))
		return BlockType::SNOW;
	if (worldY >= -3 && worldY <= -1)
		return BlockType::WHITE_GRAVEL;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
	{
		if (slope > 2.0f)
			return BlockType::STONE;
		else
			return BlockType::DIRT;
	}
}

uint8_t MountainsBiome::splitSkyFromSea(int worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t MountainsBiome::paintSurface(HeightMap &, int , int , int worldX, int worldY, int worldZ, double slope) const
{
	if (slope < 10.0 - (15 - worldY * 0.05))
		return BlockType::SNOW;
	if (slope > 2.0f)
		return BlockType::STONE;
	double noiseValue = _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)});
	if (noiseValue <= -0.75)
		return BlockType::WHITE_GRAVEL;
	else
		return BlockType::GRASS;
}
