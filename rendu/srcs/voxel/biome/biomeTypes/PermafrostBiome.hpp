#pragma once

#include "ABiome.hpp"
#include "SimplexNoise.hpp"
#include <cmath>

class PermafrostBiome : public ABiome
{
	public:
		PermafrostBiome(uint32_t seed, std::vector<std::pair<float, float>> temperatureRange, std::vector<std::pair<float, float>> heightRange)
			: ABiome(seed, temperatureRange, heightRange, 8, -1, 2), _biomeNoise(SimplexNoise<2>(seed, 0.042f, 100000.0f)) { _biomeNoise.setFBM(2, 0.5, 2.0); };
	
		double			computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			paintSurface(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			fillWorld(const BiomePaintingInfo & paintingInfo) const override;
	
	private:
		SimplexNoise<2>	_biomeNoise;
};
