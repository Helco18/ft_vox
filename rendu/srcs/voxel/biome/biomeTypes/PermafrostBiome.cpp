#include "PermafrostBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double PermafrostBiome::computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const
{
	return _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)}) * 5.0 + _terrainHeightOffset;
}

uint8_t PermafrostBiome::fillWorld(const BiomePaintingInfo & paintingInfo) const
{
	int worldY = paintingInfo.worldY;
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});

	if (worldY > -2000 + noiseValue * 10 && worldY < -500 + noiseValue * 10)
		return BlockType::MAGMA_STONE;
	else if (worldY >= -7 - noiseValue * 2 && worldY <= 0)
		return paintingInfo.temperature < -0.2f - noiseValue * 0.05 ? BlockType::WHITE_GRAVEL : BlockType::SAND;
	else if (worldY <= -1990 + noiseValue * 10)
		return BlockType::BLACK_STONE;
	return paintingInfo.worldY > 0 ? BlockType::ICE : BlockType::STONE;
}

uint8_t PermafrostBiome::splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const
{
	return paintingInfo.worldY <= SEA_LEVEL ? BlockType::ICE : BlockType::AIR;
}

uint8_t PermafrostBiome::paintSurface(const BiomePaintingInfo & paintingInfo) const
{
	double height = paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z);
	if (height <= -0.45)
		return BlockType::WHITE_GRAVEL;
	return BlockType::SNOW;
}
