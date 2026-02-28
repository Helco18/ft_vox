#include "MesaBiome.hpp"
#include "BlockData.hpp"
#include "TerrainGenerator.hpp"

double MesaBiome::computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const
{
	double noiseValue = paintingInfo.heightMap->getHeight(paintingInfo.x, paintingInfo.z);
	if (noiseValue > 0.0 && noiseValue < 0.2)
		return static_cast<int>(std::floor(noiseValue * 150) + _terrainHeightOffset);
	else if (noiseValue >= 0.2)
		return static_cast<int>(std::floor(noiseValue * 10) + _terrainHeightOffset + 29);
	else
		return static_cast<int>(std::floor(noiseValue * 10) + _terrainHeightOffset);
}

static uint8_t paintStratification(int worldY)
{
	if (worldY >= 14 && worldY <= 16)
		return BlockType::WHITE_STONE;
	else if (worldY == 17)
		return BlockType::BROWN_STONE;
	else if (worldY >= 18 && worldY <= 19)
		return BlockType::RED_STONE;
	else if (worldY >= 20 && worldY <= 22)
		return BlockType::SANDSTONE;
	else if (worldY >= 23 && worldY <= 24)
		return BlockType::BROWN_STONE;
	else if (worldY >= 25 && worldY <= 26)
		return BlockType::SANDSTONE;
	else if (worldY == 27)
		return BlockType::WHITE_STONE;
	else if (worldY == 28)
		return BlockType::BROWN_STONE;
	else if (worldY >= 29 && worldY <= 31)
		return BlockType::RED_STONE;
	else if (worldY >= 32 && worldY <= 33)
		return BlockType::SANDSTONE;
	else if (worldY == 34)
		return BlockType::WHITE_STONE;
	else if (worldY == 35)
		return BlockType::BROWN_STONE;
	return BlockType::RED_STONE;
}

uint8_t MesaBiome::fillWorld(const BiomePaintingInfo & paintingInfo) const
{
	int worldY = paintingInfo.worldY;
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});

	if (worldY >= 14 && worldY <= 35)
		return paintStratification(worldY);
	if (worldY >= 14 && worldY <= 22)
		return BlockType::WHITE_STONE;
	else if (worldY >= 16 && worldY <= 17)
		return BlockType::RED_STONE;
	else if (worldY > (-50 + noiseValue * 5) && worldY < 0 - noiseValue * 5)
		return BlockType::SANDSTONE;
	else if (worldY < -500 + noiseValue * 10)
		return BlockType::MAGMA_STONE;
	else if (worldY <= -40)
		return BlockType::STONE;
	return BlockType::RED_STONE;
}

uint8_t MesaBiome::splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const
{
	return paintingInfo.worldY <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

uint8_t MesaBiome::paintSurface(const BiomePaintingInfo & paintingInfo) const
{
	int worldY = paintingInfo.worldY;
	double noiseValue = _biomeNoise.queryState({static_cast<double>(paintingInfo.worldX), static_cast<double>(paintingInfo.worldZ)});

	if (paintingInfo.slope > 2.0f)
	{
		if (worldY >= 14 && worldY <= 35)
			return paintStratification(worldY);
		return BlockType::RED_STONE;
	}
	else if (worldY >= 0 && worldY < (7 + noiseValue * 5))
		return BlockType::SAND;
	else if (worldY >= 2)
		return BlockType::RED_SAND;
	return SAND;
}
