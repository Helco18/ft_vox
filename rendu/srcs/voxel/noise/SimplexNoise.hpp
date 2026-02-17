#pragma once

#include "glm/glm.hpp"
#include <vector>

template <uint8_t N>
class SimplexNoise
{
	static_assert(N >= 1, "SimplexNoise: must be >= 1");

	public:
		SimplexNoise(uint32_t seed = 0, float noiseScale = 0.01f, float offset = 0.0f);
		~SimplexNoise() {};

		double		queryState(const std::array<double, N> & pos) const;
		void		setFBM(int octave, float persistance, float lacunarity) { _octave =	octave; _persistance = persistance; _lacunarity = lacunarity; }
		static void	printNoise(uint32_t seed); // DEBUG
	private:
		std::array<double, N>	_gradient(int hash) const;
		double				_dot(const std::array<double, N> & a, const std::array<double, N> & b) const;
		double				_noise(const std::array<double, N> & pos) const;

		float		_noiseScale;
		float		_offset;

		double		_F; // skew (nD to simplex)
		double		_G; // unskew (simplex to nD)

		// Fractal Brownian Motion
		int			_octave = 1;
		float		_persistance = 1.0;
		float		_lacunarity = 1.0;

		std::array<int, 512>	_perm;
};

#include "SimplexNoise.tpp"
