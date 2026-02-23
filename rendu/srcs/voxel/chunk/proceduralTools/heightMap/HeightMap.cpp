#include "HeightMap.hpp"

int HeightMap::_index(int x, int z) const
{
	return x + z * _sizeX;
}

void HeightMap::_interpolate(int step)
{
	for (int z = 0; z < _sizeZ; z += step)
	{
		for (int x = 0; x < _sizeX; x += step)
		{
			int x1 = std::min(x + step, _sizeX - 1);
			int z1 = std::min(z + step, _sizeZ - 1);

			double h00 = _data[_index(x, z)];
			double h10 = _data[_index(x1, z)];
			double h01 = _data[_index(x, z1)];
			double h11 = _data[_index(x1, z1)];

			int dxMax = x1 - x;
			int dzMax = z1 - z;

			for (int dz = 0; dz <= dzMax; ++dz)
			{
				double tz = dz / double(dzMax);
				for (int dx = 0; dx <= dxMax; ++dx)
				{
					double tx = dx / double(dxMax);
					double hx0 = h00 + (h10 - h00) * tx;
					double hx1 = h01 + (h11 - h01) * tx;
					double h   = hx0 + (hx1 - hx0) * tz;
					_data[_index(x + dx, z + dz)] = h;
				}
			}
		}
	}
}

float HeightMap::getSlope(int x, int z) const
{
	x += 2;
	z += 2;
	int xm = std::max(0, x-1);
	int xp = std::min(_sizeX-1, x+1);
	int zm = std::max(0, z-1);
	int zp = std::min(_sizeZ-1, z+1);

	float dx = _data[_index(xp, z)] - _data[_index(xm, z)];
	float dz = _data[_index(x, zp)] - _data[_index(x, zm)];

	return std::sqrt(dx*dx + dz*dz) * 0.5f;
}

void HeightMap::computeHeight(int worldX, int worldZ, const SimplexNoise<2> & noise, int step)
{
	for (int z = 0; z < _sizeZ; z += step)
	{
		for (int x = 0; x < _sizeX; x += step)
		{
			std::array<double, 2> p = {static_cast<double>(worldX + x), static_cast<double>(worldZ + z)};
			_data[_index(x, z)] = noise.queryState(p);
		}
	}

	_interpolate(step);
}
