#include "MesaBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

int	MesaBiome::computeBiomeHeight(HeightMap & heightMap, int x, int z, int, int) const
{
	double noiseValue = heightMap.getHeight(x, z);
	if (noiseValue > 0.0 && noiseValue < 0.6)
		return static_cast<int>(std::floor(noiseValue * 60) + _terrainHeightOffset);
	else if (noiseValue > 0.6)
		return static_cast<int>(std::floor(noiseValue * 10) + _terrainHeightOffset + 30);
	else
		return static_cast<int>(std::floor(noiseValue * 10) + _terrainHeightOffset);
}

uint8_t MesaBiome::fillWorld(int height, double worldY, float slope) const
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

uint8_t MesaBiome::splitSkyFromSea(double worldY) const
{
	return (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t MesaBiome::paintSurface(double worldY, float slope) const
{
	if (slope > 0.05f)
		return BlockType::STONE;
	else
		return worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}
