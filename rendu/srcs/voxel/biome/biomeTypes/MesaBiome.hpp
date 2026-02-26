#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class MesaBiome : public ABiome
{
	public:
		MesaBiome(uint32_t seed, std::pair<float, float> temperatureRange, std::pair<float, float> heightRange)
			: ABiome(seed, temperatureRange, heightRange, 10, -1, 1), _biomeNoise(SimplexNoise<2>(seed, 0.01f, 100000.0f)) {};

		double			computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			paintSurface(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			fillWorld(const BiomePaintingInfo & paintingInfo) const override;

	private:
		SimplexNoise<2>	_biomeNoise;
};
