#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class MountainsBiome : public ABiome
{
	public:
		MountainsBiome(uint32_t seed) : ABiome(seed, 20, -1, 9), _biomeNoise(SimplexNoise<2>(seed, 0.002f, 100000.0f)) { _biomeNoise.setFBM(4, 0.30, 1.8); };

		int	computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const override
		{
			double noiseValue = _biomeNoise.queryState({static_cast<double>(worldX), static_cast<double>(worldZ)});
			if (noiseValue > -0.5 && noiseValue <= 0.5)
				noiseValue = noiseValue * 110 + 45;
			else if (noiseValue > 0.5)
				noiseValue = noiseValue * 200 + 0;
			else
				noiseValue = noiseValue * 40 + 10;
			return static_cast<int>(std::floor(noiseValue) + _terrainHeightOffset);
		}

	private:
		SimplexNoise<2>	_biomeNoise;
};
