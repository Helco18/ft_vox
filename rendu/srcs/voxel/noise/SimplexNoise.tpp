#include "SimplexNoise.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <numeric>
#include <random>
#include <algorithm>
#include "stb/stb_image_write.h"

template <uint8_t N>
void SimplexNoise<N>::printNoise(uint32_t seed)
{
	SimplexNoise<N> noise(seed, 0.005f, 100000.0);

	std::vector<uint8_t> data;

	for (double x = 0; x < 1024; ++x)
		for (double y = 0; y < 1024; ++y)
			data.push_back(static_cast<uint8_t>(std::round(255 * (((noise.queryState({x, y}) + 1.0) / 2.0)))));
	
	stbi_write_png("noise.png", 1024, 1024, 1, data.data(), 1024);
}

template <uint8_t N>
SimplexNoise<N>::SimplexNoise(uint32_t seed, float noiseScale, float offset): _noiseScale(noiseScale), _offset(offset)
{
	const double n = static_cast<double>(N);

	_F = (std::sqrt(n + 1.0) - 1.0) / n;
	_G = (1.0 - 1.0 / std::sqrt(n + 1.0)) / n;

	std::array<uint8_t, 256> p;
	std::iota(p.begin(), p.end(), 0);

	std::mt19937 rng(seed);
	std::shuffle(p.begin(), p.end(), rng);

	for (uint16_t i = 0; i < 512; ++i)
		_perm[i] = p[i & 255];
}

template <uint8_t N>
double SimplexNoise<N>::queryState(const std::vector<double> & pos) const
{
	std::vector<double> scalePos(N);
	for (uint8_t d = 0; d < N; ++d)
	{
		scalePos[d] = pos[d] * _noiseScale + _offset;
		if (scalePos[d] <= 0)
			scalePos[d] *= -1;
	}
	double value = 0.0;

	double frequency = 1.0;
	double amplitude = 1.0;

	for (int octave = 0; octave < _octave; ++octave)
	{
		for (uint8_t d = 0; d < N; ++d)
			scalePos[d] *= frequency;
		value += _noise(scalePos) * amplitude;
		amplitude *= _persistance;
		frequency *= _lacunarity;
	}
	return value;
}

template <uint8_t N>
double SimplexNoise<N>::_noise(const std::vector<double> & pos) const
{
	double value = 0.0;

	// first corner : x0
	// find the first corner of the simplex
	std::vector<double> x0(N);

	// simplex cord
	std::vector<double> i(N);

	// SKEW :
	double s = 0.0;
	for (double v : pos)
		s += v;
	s *= _F;

	double isum = 0.0;
	for (uint8_t d = 0; d < N; ++d)
	{
		i[d] = std::floor(pos[d] + s);
		isum += i[d];
	}

	// UNSKEW :
	for (uint8_t d = 0; d < N; ++d)
		x0[d] = pos[d] - i[d] + isum * _G;

	// détermination des sommets
	std::vector<int> rank(N);
	std::iota(rank.begin(), rank.end(), 0);
	std::sort(rank.begin(), rank.end(), [&x0](int a, int b){ return (x0[a] > x0[b]); });

	// contribution des sommets
	for (uint8_t corner = 0; corner <= N; ++corner)
	{
		// sélection de l'offset pour les angles (corner)
		std::vector<double> offset(N, 0.0);
		for (uint8_t d = 0; d < corner; ++d)
			offset[rank[d]] = 1.0;

		// calcul des angles :
		std::vector<double> x(N);
		for (uint8_t d = 0; d < N; ++d)
			x[d] = x0[d] - offset[d] + static_cast<double>(corner) * _G;

		// sélection du hash en fonction de la position de l'angle
		int hash = 0;
		int h = 0;
		for (uint8_t d = 0; d < N; ++d)
			h = _perm[(h + static_cast<int>(std::floor(i[d] + offset[d]))) & 255];
		hash = _perm[h];

		// atténuation de la distance :
		double t = 0.5;
		for (double v : x)
			t -= v * v; 

		// application de la déformation sur les valeurs non null
		if (t > 0.0)
		{
			t *= t;
			value += t * t * _dot(_gradient(hash), x);
		}
	}
	if constexpr (N == 2)
		return 70.6 * value;
	else if constexpr (N == 3)
		return 32.0 * value;
	return 70.6 * value;
}

template <uint8_t N>
std::vector<double>	SimplexNoise<N>::_gradient(int hash) const
{

	// std::vector<double> g(N);
	// double len = 0.0;
	// for (uint8_t d = 0; d < N; ++d)
	// {
    //     g[d] = static_cast<double>(hash) / 127.5 - 1.0;
	// 	len += g[d] * g[d];
	// }

	// len = 1.0 / std::sqrt(len);
	// for (double & v : g)
	// 	v *= len;

	if (N == 2)
	{
		static const double grad2[8][2] = {
			{1.0,1.0}, {-1.0,1.0}, {1.0,-1.0}, {-1.0,-1.0},
			{1.0,0.0}, {-1.0,0.0}, {0.0,1.0}, {0.0,-1.0}
		};
		std::vector<double> g(2);
	    int h = hash & 7;
	    g[0] = grad2[h][0];
	    g[1] = grad2[h][1];

	    return g;
	}
	else if (N == 3)
	{
		static const double grad3[12][3] = {
			{1,1,0}, {-1,1,0}, {1,-1,0}, {-1,-1,0},
			{1,0,1}, {-1,0,1}, {1,0,-1}, {-1,0,-1},
			{0,1,1}, {0,-1,1}, {0,1,-1}, {0,-1,-1}
		};
		std::vector<double> g(3);
	    int h = hash % 12;
	    g[0] = grad3[h][0];
	    g[1] = grad3[h][1];
	    g[2] = grad3[h][2];

	    return g;
	}

	static const double grad2[8][2] = {
		{1.0,1.0}, {-1.0,1.0}, {1.0,-1.0}, {-1.0,-1.0},
		{1.0,0.0}, {-1.0,0.0}, {0.0,1.0}, {0.0,-1.0}
	};
	std::vector<double> g(2);
    int h = hash & 7;
    g[0] = grad2[h][0];
    g[1] = grad2[h][1];

    return g;
}

template <uint8_t N>
double	SimplexNoise<N>::_dot(const std::vector<double> & a, const std::vector<double> & b) const
{
	double r = 0.0;
	for (uint8_t d = 0; d < N; ++d)
		r += a[d] * b[d];
	return r;
}
