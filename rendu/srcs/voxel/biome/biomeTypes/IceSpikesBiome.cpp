#include "IceSpikesBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

int	IceSpikesBiome::computeBiomeHeight(HeightMap & heightMap, int x, int z, int worldX, int worldZ) const
{
	double noiseValue = _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 60;
	if (noiseValue > 50)
		return static_cast<int>(std::floor(heightMap.getHeight(x, z) * 10) + _terrainHeightOffset) + noiseValue - 50;
	return static_cast<int>(std::floor(heightMap.getHeight(x, z) * 10) + _terrainHeightOffset);
}

uint8_t IceSpikesBiome::fillWorld(int height, double worldY, float) const
{
	if (worldY <= height - 2 - (height % 2))
		return worldY > 0 ? BlockType::ICE : BlockType::STONE;
	else
		return BlockType::SNOW;
}

uint8_t IceSpikesBiome::splitSkyFromSea(double worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t IceSpikesBiome::paintSurface(double, float) const
{
	return BlockType::SNOW;
}
