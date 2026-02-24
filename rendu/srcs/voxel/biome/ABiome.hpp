#pragma once

#include "HeightMap.hpp"
#include <iterator>

class ABiome
{
	public:
		ABiome(uint32_t seed, int terrainHeightOffset, int y1, int y2) :
			_seed(seed), _terrainHeightOffset(terrainHeightOffset), _chunkHeightClamp(std::pair<int, int>(y1, y2)) {}

		inline bool			isWithinRange(int y) const
		{
			return (y >= _chunkHeightClamp.first && y <= _chunkHeightClamp.second);
		}

		virtual int			computeBiomeHeight(HeightMap & heightMap, int x, int z, int worldX, int worldZ) const = 0;

	protected:
		typedef std::pair<int, int>	ChunkHeightClamp;

		uint32_t			_seed;
		int					_terrainHeightOffset;
		ChunkHeightClamp	_chunkHeightClamp;
};
