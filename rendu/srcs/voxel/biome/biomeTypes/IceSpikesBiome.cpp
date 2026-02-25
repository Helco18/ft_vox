#include "IceSpikesBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double IceSpikesBiome::computeBiomeHeight(HeightMap & heightMap, int x, int z, int worldX, int worldZ) const
{
	double noiseValue = _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 60;
	if (noiseValue > 50)
		return heightMap.getHeight(x, z) * 10.0 + _terrainHeightOffset + noiseValue - 50;
	return heightMap.getHeight(x, z) * 10.0 + _terrainHeightOffset;
}

uint8_t IceSpikesBiome::fillWorld(int, int, int, int worldY, double) const
{
	return worldY > 0 ? BlockType::ICE : BlockType::STONE;
}

uint8_t IceSpikesBiome::splitSkyFromSea(int worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::ICE : BlockType::AIR;
}

uint8_t IceSpikesBiome::paintSurface(HeightMap & heightMap, int x, int z, int, int, int, double) const
{
	double height = heightMap.getHeight(x, z);
	if (height <= -0.45)
		return BlockType::WHITE_GRAVEL;
	return BlockType::SNOW;
}
