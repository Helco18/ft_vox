#include "SimplexNoise.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <numeric>
#include <random>

template <uint8_t N>
SimplexNoise<N>::SimplexNoise(uint32_t seed): _seed(seed)
{
	const double n = static_cast<double>(N);
	_F = (std::sqrt(n + 1.0) - 1.0) / n;
	_G = (1.0 - 1.0 / std::sqrt(n + 1.0)) / n;


	std::array<int, 256> p;
	std::iota(p.begin(), p.end(), 0);

	std::mt19937 rng(seed);
	std::shuffle(p.begin(), p.end(), rng);

	for (size_t i = 0; i < 512; ++i)
		_perm[i] = p[i & 255];
}

template <uint8_t N>
double SimplexNoise<N>::queryState(const std::vector<double> & pos) const
{
	double value = 0.0;
	// first corner : x0
	// finde the firs corner of the simplex

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
		i[d] = floor(pos[d] + s);
		isum += i[d];
	}

	// UNSKEW :
	for (uint8_t d = 0; d < N; ++d)
		x0[d] = pos[d] - i[d] + isum * _G;

	// determination des somet
	std::vector<double> rank(N, 0.0);
	for (uint8_t a = 0; a < N; ++a)
	{
		for (uint8_t b = 0; b < N; ++b)
		{
			if (x0[a] > x0[b])
				rank[a]++;
		}
	}

	// contribution des somet
	for (uint8_t corner = 0; corner <= N; ++corner)
	{
		std::vector<double> x(N);
		int hash = 0;

		for (uint8_t d = 0; d < N; ++d)
		{
			int offset = rank[d] >= N - corner ? 1 : 0;
			x[d] = x0[d] - static_cast<double>(offset) + static_cast<double>(corner) * _G;

			// la magie !!! :
			hash = _perm[(hash + static_cast<int>(i[d]) + offset) & 255];
		}

		// attenuation de la distance :
		double t = 0.5 * N;
		// double t = 0.5;
		for (double v : x)
			t -= v * v; 

		// aplication de la deformation sur les valeu non null
		// Logger::log(VOXEL, DEBUG, "t val :" + toString(t));
		if (t > 0.0)
		{
			t *= t;
			value += t * t * _dot(_gradient(hash), x);
		}
	}

	Logger::log(VOXEL, DEBUG, "noise value :" + toString(value));
	return value;
}

template <uint8_t N>
std::vector<double>	SimplexNoise<N>::_gradient(int hash) const
{
	std::vector<double> g(N);
	double len = 0.0;
	for (uint8_t d = 0; d < N; ++d)
	{
		g[d] = static_cast<double>(hash) / 127.5 - 1.0;
		len += g[d] * g[d];
	}

	len = 1.0 / std::sqrt(len);
	for (double & v : g)
		v *= len;

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
