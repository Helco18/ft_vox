#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class TundraBiome : public ABiome
{
	public:
		TundraBiome(uint32_t seed) : ABiome(seed, 8, -1, 1), _biomeNoise(SimplexNoise<2>(seed, 0.01f, 100000.0f)) { _biomeNoise.setFBM(2, 0.5, 2.0); };

		int				computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const override;
		uint8_t			paintSurface(double worldY, float slope) const override;
		uint8_t			splitSkyFromSea(double worldY) const override;
		uint8_t			fillWorld(int height, double worldY, float slope) const override;

	private:
		SimplexNoise<2>	_biomeNoise;
};
