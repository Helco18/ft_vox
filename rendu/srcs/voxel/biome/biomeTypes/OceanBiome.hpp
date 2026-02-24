#pragma once

#include "ABiome.hpp"
#include <cmath>

class OceanBiome : public ABiome
{
	public:
		OceanBiome(uint32_t seed) : ABiome(seed, -20, -2, 0) {};

		int	computeBiomeHeight(HeightMap & heightMap, int x, int z, int, int) const override
		{
			return static_cast<int>(std::floor(heightMap.getHeight(x, z) * 10) + _terrainHeightOffset);
		}

};
