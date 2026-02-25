#include "DesertBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double DesertBiome::computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const
{
	return _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 8 + _terrainHeightOffset;
}

uint8_t DesertBiome::fillWorld(int, int, int, int worldY, double) const
{
	if (worldY >= -50)
		return BlockType::SANDSTONE;
	else if (worldY < -50)
		return BlockType::STONE;
	else
		return BlockType::SAND;
}

uint8_t DesertBiome::splitSkyFromSea(int worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t DesertBiome::paintSurface(HeightMap &, int, int, int, int, int, double) const
{
	return BlockType::SAND;
}
