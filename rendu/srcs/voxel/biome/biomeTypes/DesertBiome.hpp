#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class DesertBiome : public ABiome
{
	public:
		DesertBiome(uint32_t seed) : ABiome(seed, 11, -1, 1), _biomeNoise(SimplexNoise<2>(seed, 0.01f, 100000.0f)) { _biomeNoise.setFBM(2, 0.5, 2.0); };
	
		int	computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const override
		{
			return static_cast<int>(std::floor(_biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 8) + _terrainHeightOffset);
		}
	
	private:
		SimplexNoise<2>	_biomeNoise;
};
