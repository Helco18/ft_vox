#pragma once

#include "ABiome.hpp"
#include <cmath>

class OceanBiome : public ABiome
{
	public:
		OceanBiome(uint32_t seed) : ABiome(seed, -20, -2, 0) {};

		int				computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const override;
		uint8_t			paintSurface(double worldY, float slope) const override;
		uint8_t			splitSkyFromSea(double worldY) const override;
		uint8_t			fillWorld(int height, double worldY, float slope) const override;

};
