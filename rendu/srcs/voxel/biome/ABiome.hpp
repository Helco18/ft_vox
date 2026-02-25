#pragma once

#include "HeightMap.hpp"
#include <iterator>

class ABiome
{
	public:
		ABiome(uint32_t seed, std::pair<float, float> temperatureRange, std::pair<float, float> heightRange, int terrainHeightOffset, int y1, int y2) :
			_seed(seed), _temperatureRange(temperatureRange), _heightRange(heightRange), _terrainHeightOffset(terrainHeightOffset),
			_chunkHeightClamp(std::pair<int, int>(y1, y2)) {}
		virtual ~ABiome() = default;

		inline bool							isWithinRange(int y) const { return (y >= _chunkHeightClamp.first && y <= _chunkHeightClamp.second); }

		virtual double						computeBiomeHeight(HeightMap & heightMap, int x, int z, int worldX, int worldZ) const = 0;
		virtual uint8_t						paintSurface(HeightMap & heightMap, int x, int z, int worldX, int worldY, int worldZ, double slope) const = 0;
		virtual uint8_t						splitSkyFromSea(int worldY) const = 0;
		virtual uint8_t						fillWorld(int worldX, int worldZ, int height, int worldY, double slope) const = 0;

		const std::pair<float, float> &		getTemperatureRange() const { return _temperatureRange; }
		const std::pair<float, float> &		getHeightRange() const { return _heightRange; }

	protected:
		uint32_t							_seed;
		std::pair<float, float>				_temperatureRange;
		std::pair<float, float>				_heightRange;
		int									_terrainHeightOffset;
		std::pair<int, int>					_chunkHeightClamp;
};
