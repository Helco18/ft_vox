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
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});

	if (worldY >= -2 - noiseValue * 2 && worldY <= 0)
		return paintingInfo.temperature < -0.2f - noiseValue * 0.05 ? BlockType::WHITE_GRAVEL : BlockType::SAND;
	else if (worldY < -500 + noiseValue * 10)
		return BlockType::MAGMA_STONE;
	else if (worldY < -4 - noiseValue * 5 && worldY > -20 - noiseValue * 5)
		return BlockType::WHITE_GRAVEL;
	else if (worldY < -20 - noiseValue * 5 && worldY > -30 - noiseValue * 5)
		return BlockType::DIRT;
	else
		return BlockType::STONE;
}

uint8_t OceanBiome::splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const
{
	return paintingInfo.worldY <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t OceanBiome::paintSurface(const BiomePaintingInfo & paintingInfo) const
{
	return paintingInfo.worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
