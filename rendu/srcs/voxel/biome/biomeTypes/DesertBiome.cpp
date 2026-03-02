#include "DesertBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double DesertBiome::computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const
{
	return _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)}) * 8 + _terrainHeightOffset;
}

uint8_t DesertBiome::fillWorld(const BiomePaintingInfo & paintingInfo) const
{
	int worldY = paintingInfo.worldY;
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});

	if (worldY >= -50)
		return BlockType::SANDSTONE;
	else if (worldY >= -7 - noiseValue * 2 && worldY <= 3 && paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z) < 3)
		return paintingInfo.temperature < -0.2f - noiseValue * 0.05 ? BlockType::WHITE_GRAVEL : BlockType::SAND;
	else if (worldY > -2000 + noiseValue * 10 && worldY < -500 + noiseValue * 10)
		return BlockType::MAGMA_STONE;
	else if (worldY <= -1990 + noiseValue * 10)
		return BlockType::BLACK_STONE;
	else if (worldY < -50)
		return BlockType::STONE;
	else
		return BlockType::SAND;
}

uint8_t DesertBiome::splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const
{
	return (paintingInfo.worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t DesertBiome::paintSurface(const BiomePaintingInfo &) const
{
	return BlockType::SAND;
}
