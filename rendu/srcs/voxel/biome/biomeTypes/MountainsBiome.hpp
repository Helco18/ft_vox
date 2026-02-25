#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class MountainsBiome : public ABiome
{
	public:
		MountainsBiome(uint32_t seed) : ABiome(seed, 20, -1, 9), _biomeNoise(SimplexNoise<2>(seed, 0.002f, 100000.0f)) { _biomeNoise.setFBM(4, 0.30, 1.8); };

		double			computeBiomeHeight(HeightMap &, int, int, int worldX, int worldZ) const override;
		uint8_t			paintSurface(HeightMap & heightMap, int x, int z, int worldX, int worldY, int worldZ, double slope) const override;
		uint8_t			splitSkyFromSea(int worldY) const override;
		uint8_t			fillWorld(int worldX, int worldZ, int height, int worldY, double slope) const override;

	private:
		SimplexNoise<2>	_biomeNoise;
};
