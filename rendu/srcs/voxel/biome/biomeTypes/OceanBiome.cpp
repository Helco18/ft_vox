#include "OceanBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double	OceanBiome::computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const
{
	return paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z) * 10.0 + _terrainHeightOffset;
}

uint8_t OceanBiome::fillWorld(const BiomePaintingInfo & paintingInfo) const
{
	int worldY = paintingInfo.worldY;
	int height = paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z);

	if (worldY >= -3 && worldY <= -1)
		return BlockType::SAND;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
		return BlockType::DIRT;
}

uint8_t OceanBiome::splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const
{
	return paintingInfo.worldY <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t OceanBiome::paintSurface(const BiomePaintingInfo & paintingInfo) const
{
	return paintingInfo.worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
