#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class IceSpikesBiome : public ABiome
{
	public:
		IceSpikesBiome(uint32_t seed) : ABiome(seed, 8, -1, 2), _biomeNoise(SimplexNoise<2>(seed, 0.042f, 100000.0f)) { _biomeNoise.setFBM(2, 0.5, 2.0); };
	
		int	computeBiomeHeight(HeightMap & heightMap, int x, int z, int worldX, int worldZ) const override
		{
			double noiseValue = _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 60;
			if (noiseValue > 50)
				return static_cast<int>(std::floor(heightMap.getHeight(x, z) * 10) + _terrainHeightOffset) + noiseValue - 50;
			return static_cast<int>(std::floor(heightMap.getHeight(x, z) * 10) + _terrainHeightOffset);
		}
	
	private:
		SimplexNoise<2>	_biomeNoise;
};
