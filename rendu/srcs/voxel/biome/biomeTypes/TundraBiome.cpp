#include "TundraBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double TundraBiome::computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const
{
	return _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 5.0 + _terrainHeightOffset;
}

uint8_t TundraBiome::fillWorld(int, int, int height, int worldY, double) const
{
	if (worldY >= -3 && worldY <= -1)
		return BlockType::WHITE_GRAVEL;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
		return BlockType::SNOW;
}

uint8_t TundraBiome::splitSkyFromSea(int worldY) const
{
	if (worldY == SEA_LEVEL)
		return BlockType::ICE;
	return (worldY) < SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t TundraBiome::paintSurface(HeightMap &, int, int, int worldX, int, int worldZ, double) const
{
	double noiseValue = _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 5;
	if (noiseValue <= -5.8)
		return BlockType::WHITE_GRAVEL;
	return BlockType::SNOW;
}

