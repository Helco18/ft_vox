#pragma once

#include "glm/glm.hpp"
#include "SimplexNoise.hpp"
#include <vector>

class HeightMap
{
	public:
		HeightMap(int sizeX, int sizeZ): _sizeX(sizeX + 4), _sizeZ(sizeZ + 4), _data(_sizeX * _sizeZ, 0.0f) {};
		~HeightMap() {};

		float				getHeight(int x, int z) const { return _data[_index(x + 2, z + 2)]; }

		void				computeHeight(int worldX, int worldZ, const SimplexNoise<2> & noise, int step);

		float				getSlope(int x, int z) const;

	private:
		int					_index(int x, int z) const;
		void				_interpolate(int step);

		int					_sizeX;
		int					_sizeZ;
		std::vector<float>	_data;
};
