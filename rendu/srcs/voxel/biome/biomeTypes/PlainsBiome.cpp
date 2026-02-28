#include "PlainsBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double PlainsBiome::computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const
{
	return _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)}) * 5.0 + _terrainHeightOffset;
}

uint8_t PlainsBiome::fillWorld(const BiomePaintingInfo & paintingInfo) const
{
	int worldY = paintingInfo.worldY;
	int height = paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z);
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});

	if (paintingInfo.slope > 2.0f)
		return BlockType::STONE;
	if (worldY >= -3 && worldY <= -1)
		return BlockType::SAND;
	else if (worldY < -500 + noiseValue * 10)
		return BlockType::MAGMA_STONE;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
		return BlockType::DIRT;
}

uint8_t PlainsBiome::splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const
{
	return (paintingInfo.worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t PlainsBiome::paintSurface(const BiomePaintingInfo & paintingInfo) const
{
	if (paintingInfo.slope > 2.0f)
		return BlockType::STONE;
	return paintingInfo.worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
