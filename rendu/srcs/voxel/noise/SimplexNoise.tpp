#include "SimplexNoise.hpp"

template <uint8_t N>
SimplexNoise<N>::SimplexNoise(uint32_t seed): _seed(seed)
{
	const double n = static_cast<double>(N);
	_f = (std::sqrt(n + 1) - 1) / n;
	_g = (n + 1 - std::sqrt(n + 1)) / n * (n + 1);
}

template <uint8_t N>
double SimplexNoise<N>::queryState(const std::vector<double> & pos) const
{
	if ((pos[1] <= (-1 + (std::sin(pos[0] / 5.0) * 5.0 +
			(std::cos((pos[2]) / 5.0) * 5.0))) ))
		return 1;
	return 0;
}
