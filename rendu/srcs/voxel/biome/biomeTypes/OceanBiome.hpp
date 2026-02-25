#pragma once

#include "ABiome.hpp"
#include <cmath>

class OceanBiome : public ABiome
{
	public:
		OceanBiome(uint32_t seed, std::pair<float, float> temperatureRange, std::pair<float, float> heightRange)
			: ABiome(seed, temperatureRange, heightRange, -20, -2, 0) {};

		double			computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const override;
		uint8_t			paintSurface(HeightMap & heightMap, int x, int z, int worldX, int worldY, int worldZ, double slope) const override;
		uint8_t			splitSkyFromSea(int worldY) const override;
		uint8_t			fillWorld(int worldX, int worldZ, int height, int worldY, double slope) const override;

};
