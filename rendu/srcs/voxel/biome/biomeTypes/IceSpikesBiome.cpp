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

uint8_t IceSpikesBiome::fillWorld(int height, double worldY, float slope) const
{
	if (worldY >= -3 && worldY <= -1)
		return BlockType::SAND;
	else if (worldY <= height - 2 - (height % 2))
		return BlockType::STONE;
	else
	{
		if (slope > 0.05f)
			return BlockType::STONE;
		else
			return BlockType::DIRT;
	}
}

uint8_t IceSpikesBiome::splitSkyFromSea(double worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t IceSpikesBiome::paintSurface(double worldY, float slope) const
{
	if (slope > 0.05f)
		return BlockType::STONE;
	else
		return worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
