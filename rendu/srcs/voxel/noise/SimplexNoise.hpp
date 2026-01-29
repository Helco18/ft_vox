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
		std::vector<double>	_gradient(int hash) const;
		double	_dot(const std::vector<double> & a, const std::vector<double> & b) const;

		uint32_t	_seed;

		float		_F; // skew (nD to simplex)
		float		_G; // unskew (simplex to nD)

		std::array<int, 512>	_perm;
};

#include "SimplexNoise.tpp"
