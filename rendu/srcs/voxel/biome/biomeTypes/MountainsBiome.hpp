#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class MountainsBiome : public ABiome
{
	public:
		MountainsBiome(uint32_t seed) : ABiome(seed, 20, -1, 9), _biomeNoise(SimplexNoise<2>(seed, 0.002f, 100000.0f)) { _biomeNoise.setFBM(4, 0.30, 1.8); };

		int				computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const override;
		uint8_t			paintSurface(double worldY, float slope) const override;
		uint8_t			splitSkyFromSea(double worldY) const override;
		uint8_t			fillWorld(int height, double worldY, float slope) const override;

	private:
		SimplexNoise<2>	_biomeNoise;
};
