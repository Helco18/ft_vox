#include "BiomeManager.hpp"
#include "CustomExceptions.hpp"
#include "OceanBiome.hpp"
#include "utils.hpp"

BiomeManager::BiomeAtlas BiomeManager::_biomeAtlas;

void BiomeManager::init()
{
	_biomeAtlas.try_emplace(OCEAN, std::make_unique<OceanBiome>());
}

const ABiome & BiomeManager::getBiome(BiomeType biomeType)
{
	if (biomeType >= _biomeAtlas.size())
		throw VoxelException("Couldn't get biome type: " + toString(biomeType) + ".");
	return *_biomeAtlas[biomeType];
}
