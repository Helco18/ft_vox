#pragma once

#include <iterator>

class ABiome
{
	public:
		ABiome(int terrainHeightOffset, int y1, int y2) : _terrainHeightOffset(terrainHeightOffset), _chunkHeightClamp(std::pair<int, int>(y1, y2)) {}

		virtual inline bool isWithinRange(int y) const = 0;
		virtual int			computeBiomeHeight(double noiseValue) const = 0;

	protected:
		typedef std::pair<int, int>	ChunkHeightClamp;

		int					_terrainHeightOffset;
		ChunkHeightClamp	_chunkHeightClamp;
};
