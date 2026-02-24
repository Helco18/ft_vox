#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class MesaBiome : public ABiome
{
	public:
		MesaBiome(uint32_t seed) : ABiome(seed, 10, -2, 2) {};

		int	computeBiomeHeight(HeightMap & heightMap, int x, int z, int, int) const override
		{
			double noiseValue = heightMap.getHeight(x, z);
			if (noiseValue > 0.0 && noiseValue < 0.6)
				return static_cast<int>(std::floor(noiseValue * 60) + _terrainHeightOffset);
			else if (noiseValue > 0.6)
				return static_cast<int>(std::floor(noiseValue * 10) + _terrainHeightOffset + 30);
			else
				return static_cast<int>(std::floor(noiseValue * 10) + _terrainHeightOffset);
		}

};
