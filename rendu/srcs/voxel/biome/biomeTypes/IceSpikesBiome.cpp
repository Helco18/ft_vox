#include "IceSpikesBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double IceSpikesBiome::computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const
{
	double height = paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z);
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)}) * 60;

	if (noiseValue > 50)
		return height * 10.0 + _terrainHeightOffset + noiseValue - 50;
	return height * 10.0 + _terrainHeightOffset;
}

uint8_t IceSpikesBiome::fillWorld(const BiomePaintingInfo & paintingInfo) const
{
	int worldY = paintingInfo.worldY;
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});

	if (worldY < -500 + noiseValue * 10)
		return BlockType::MAGMA_STONE;
	return paintingInfo.worldY > 0 ? BlockType::ICE : BlockType::STONE;
}

uint8_t IceSpikesBiome::splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const
{
	return paintingInfo.worldY <= SEA_LEVEL ? BlockType::ICE : BlockType::AIR;
}

uint8_t IceSpikesBiome::paintSurface(const BiomePaintingInfo & paintingInfo) const
{
	double height = paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z);
	if (height <= -0.45)
		return BlockType::WHITE_GRAVEL;
	return BlockType::SNOW;
}
