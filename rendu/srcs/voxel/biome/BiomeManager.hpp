#pragma once

#include <unordered_map>
#include "ABiome.hpp"
#include <memory>

enum BiomeType
{
	OCEAN,
	PLAINS,
	DESERT,
	MESA,
	PERMAFROST,
	ICE_SPIKES,
	TUNDRA,
	MOUNTAINS
};

struct BiomeDistanceInfo
{
	float		distance = 0.0f;
	ABiome *	biome = nullptr;
};

class BiomeManager
{
	public:
		BiomeManager() = delete;
		~BiomeManager() = delete;

		static void								init(uint32_t seed);
		static const ABiome &					getBiome(BiomeType biomeType);
		static const ABiome &					getBiomeAt(float temperature, float height);
		static std::vector<BiomeDistanceInfo>	getBiomeSamples(float temperature, float height);

	private:
		typedef std::unordered_map<BiomeType, std::unique_ptr<ABiome>>	BiomeAtlas;

		static BiomeAtlas						_biomeAtlas;
};
