#pragma once

#include "glm/glm.hpp"
#include <vector>

template <uint8_t N>
class SimplexNoise
{
	static_assert(N >= 1, "SimplexNoise: must be >= 1");

	public:
		explicit	SimplexNoise(uint32_t seed = 0);
		~SimplexNoise() {};

		double		queryState(const std::vector<double> & pos) const;
	private:
		uint32_t	_seed;

		float		_f; // skew (nD to simplex)
		float		_g; // unskew (simplex to nD)

};

#include "SimplexNoise.tpp"
