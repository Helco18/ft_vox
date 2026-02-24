#pragma once

#include "ABiome.hpp"
#include <cmath>

class OceanBiome : public ABiome
{
	public:
		OceanBiome() : ABiome(-20, -2, 0) {};

		inline bool isWithinRange(int y) const override
		{
			return (y >= _chunkHeightClamp.first && y <= _chunkHeightClamp.second);
		}
	
		int	computeBiomeHeight(double noiseValue) const override
		{
			return static_cast<int>(std::floor(noiseValue * 10) + _terrainHeightOffset);
		}

};
