#pragma once

#include "ABiome.hpp"
#include <cmath>

class OceanBiome : public ABiome
{
	public:
		OceanBiome(uint32_t seed, std::pair<float, float> temperatureRange, std::pair<float, float> heightRange)
			: ABiome(seed, temperatureRange, heightRange, -20, -2, 0) {};

		double			computeBiomeHeight(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			paintSurface(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			splitSkyFromSea(const BiomePaintingInfo & paintingInfo) const override;
		uint8_t			fillWorld(const BiomePaintingInfo & paintingInfo) const override;

};
