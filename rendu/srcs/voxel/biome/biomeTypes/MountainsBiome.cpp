#include "MountainsBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double MountainsBiome::computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const
{
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});
	if (noiseValue > -0.5 && noiseValue <= 0.5)
		noiseValue = noiseValue * 110 + 45;
	else if (noiseValue > 0.5)
		noiseValue = noiseValue * 200 + 0;
	else
		noiseValue = noiseValue * 40 + 10;
	return noiseValue + _terrainHeightOffset;
}

uint8_t MountainsBiome::fillWorld(const BiomePaintingInfo & paintingInfo) const
{
	int worldY = paintingInfo.worldY;
	int height = paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z);
	double slope = paintingInfo.slope;
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});

	if (slope < 10.0 - (15 - worldY * 0.05) && worldY > height - 2 - (height % 2))
		return BlockType::SNOW;
	else if (worldY >= -7 - noiseValue * 2 && worldY <= 3 && paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z) < 3)
		return paintingInfo.temperature < -0.2f - noiseValue * 0.05 ? BlockType::WHITE_GRAVEL : BlockType::SAND;
	else if (worldY > -2000 + noiseValue * 10 && worldY < -500 + noiseValue * 10)
		return BlockType::MAGMA_STONE;
	else if (worldY <= -1990 + noiseValue * 10)
		return BlockType::BLACK_STONE;
	else if (worldY <= height - 2 - (height % 2))
	{
		// if (paintingInfo.temperature >= 0.5)
		// 	return BlockType::RED_STONE;
		return BlockType::STONE;
	}
	else
	{
		if (slope > 2.0f)
		{
			if (paintingInfo.temperature >= 0.5)
				return BlockType::RED_STONE;
			return BlockType::STONE;
		}
		else
		{
			if (paintingInfo.temperature >= 0.5)
				return BlockType::SAND;
			else if (paintingInfo.temperature <= -0.5)
				return BlockType::SNOW;
			return BlockType::DIRT;
		}
	}
}

uint8_t MountainsBiome::splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const
{
	return paintingInfo.worldY <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t MountainsBiome::paintSurface(const BiomePaintingInfo & paintingInfo) const
{
	double slope = paintingInfo.slope;

	if (slope < 10.0 - (15 - paintingInfo.worldY * 0.05))
		return BlockType::SNOW;
	if (slope > 2.0f)
	{
		if (paintingInfo.temperature >= 0.5)
			return BlockType::RED_STONE;
		return BlockType::STONE;
	}
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});
	if (noiseValue <= -0.75)
		return BlockType::WHITE_GRAVEL;
	else if (paintingInfo.temperature >= 0.5)
		return BlockType::SAND;
	else if (paintingInfo.temperature <= -0.5)
		return BlockType::SNOW;
	else
		return BlockType::GRASS;
}
