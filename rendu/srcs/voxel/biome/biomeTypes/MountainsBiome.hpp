#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class MountainsBiome : public ABiome
{
	public:
		MountainsBiome(uint32_t seed, std::pair<float, float> temperatureRange, std::pair<float, float> heightRange)
			: ABiome(seed, temperatureRange, heightRange, 20, -1, 9), _biomeNoise(SimplexNoise<2>(seed, 0.002f, 100000.0f)) { _biomeNoise.setFBM(4, 0.30, 1.8); };

		double			computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			paintSurface(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			fillWorld(const BiomePaintingInfo & paintingInfo) const override;

	private:
		SimplexNoise<2>	_biomeNoise;
};
