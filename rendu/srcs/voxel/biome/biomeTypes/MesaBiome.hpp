#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class MesaBiome : public ABiome
{
	public:
		MesaBiome(uint32_t seed) : ABiome(seed, 10, -2, 2) {};

		int				computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const override;
		uint8_t			paintSurface(double worldY, float slope) const override;
		uint8_t			splitSkyFromSea(double worldY) const override;
		uint8_t			fillWorld(int height, double worldY, float slope) const override;

};
