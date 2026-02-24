#include "BiomeManager.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"
#include "OceanBiome.hpp"
#include "PlainsBiome.hpp"
#include "DesertBiome.hpp"
#include "MesaBiome.hpp"
#include "IceSpikesBiome.hpp"
#include "TundraBiome.hpp"
#include "MountainsBiome.hpp"

BiomeManager::BiomeAtlas BiomeManager::_biomeAtlas;

void BiomeManager::init(uint32_t seed)
{
	_biomeAtlas.try_emplace(OCEAN, std::make_unique<OceanBiome>(seed));
	_biomeAtlas.try_emplace(PLAINS, std::make_unique<PlainsBiome>(seed));
	_biomeAtlas.try_emplace(DESERT, std::make_unique<DesertBiome>(seed));
	_biomeAtlas.try_emplace(MESA, std::make_unique<MesaBiome>(seed));
	_biomeAtlas.try_emplace(ICE_SPIKES, std::make_unique<IceSpikesBiome>(seed));
	_biomeAtlas.try_emplace(TUNDRA, std::make_unique<TundraBiome>(seed));
	_biomeAtlas.try_emplace(MOUNTAINS, std::make_unique<MountainsBiome>(seed));
}

const ABiome & BiomeManager::getBiome(BiomeType biomeType)
{
	if (biomeType >= _biomeAtlas.size())
		throw VoxelException("Couldn't get biome type: " + toString(biomeType) + ".");
	return *_biomeAtlas[biomeType];
}
