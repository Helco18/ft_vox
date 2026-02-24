#pragma once

#include <unordered_map>
#include "ABiome.hpp"
#include <memory>

enum BiomeType
{
	OCEAN
};

class BiomeManager
{
	public:
		BiomeManager() = delete;
		~BiomeManager() = delete;

		static void				init();
		static const ABiome &	getBiome(BiomeType biomeType);

	private:
		typedef std::unordered_map<BiomeType, std::unique_ptr<ABiome>>	BiomeAtlas;

		static BiomeAtlas		_biomeAtlas;
};
