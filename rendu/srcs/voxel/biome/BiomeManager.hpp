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
	ICE_SPIKES,
	TUNDRA,
	MOUNTAINS
};

class BiomeManager
{
	public:
		BiomeManager() = delete;
		~BiomeManager() = delete;

		static void				init(uint32_t seed);
		static const ABiome &	getBiome(BiomeType biomeType);

	private:
		typedef std::unordered_map<BiomeType, std::unique_ptr<ABiome>>	BiomeAtlas;

		static BiomeAtlas		_biomeAtlas;
};
