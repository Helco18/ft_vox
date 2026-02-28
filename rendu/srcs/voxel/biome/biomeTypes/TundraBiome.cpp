#include "TundraBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double TundraBiome::computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const
{
	return _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)}) * 5.0 + _terrainHeightOffset;
}

uint8_t TundraBiome::fillWorld(const BiomePaintingInfo & paintingInfo) const
{
	int worldY = paintingInfo.worldY;
	int height = paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z);
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});

	if (worldY >= -3 && worldY <= -1)
		return BlockType::WHITE_GRAVEL;
	else if (worldY < -500 + noiseValue * 10)
		return BlockType::MAGMA_STONE;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
		return BlockType::SNOW;
}

uint8_t TundraBiome::splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const
{
	int worldY = paintingInfo.worldY;

	if (worldY == SEA_LEVEL)
		return BlockType::ICE;
	return (worldY) < SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t TundraBiome::paintSurface(const BiomePaintingInfo & paintingInfo) const
{
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)}) * 5;
	if (noiseValue <= -5.8)
		return BlockType::WHITE_GRAVEL;
	return BlockType::SNOW;
}

