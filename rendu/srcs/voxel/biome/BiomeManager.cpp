#include "BiomeManager.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"
#include "OceanBiome.hpp"
#include "PlainsBiome.hpp"
#include "DesertBiome.hpp"
#include "MesaBiome.hpp"
#include "IceSpikesBiome.hpp"
#include "TundraBiome.hpp"
#include "MountainsBiome.hpp"

BiomeManager::BiomeAtlas BiomeManager::_biomeAtlas;

void BiomeManager::init(uint32_t seed)
{
	_biomeAtlas.try_emplace(OCEAN, std::make_unique<OceanBiome>(seed, std::pair(-1.0f, 1.0f), std::pair(-1.0f, 0.2f)));
	_biomeAtlas.try_emplace(PLAINS, std::make_unique<PlainsBiome>(seed, std::pair(-0.5f, 0.5f), std::pair(0.2f, 0.9f)));
	_biomeAtlas.try_emplace(DESERT, std::make_unique<DesertBiome>(seed, std::pair(0.5f, 1.0f), std::pair(0.2f, 0.7f)));
	_biomeAtlas.try_emplace(MESA, std::make_unique<MesaBiome>(seed, std::pair(0.5f, 1.0f), std::pair(0.7f, 0.9f)));
	_biomeAtlas.try_emplace(TUNDRA, std::make_unique<TundraBiome>(seed, std::pair(-1.0f, -0.5f), std::pair(0.2f, 0.9f)));
	_biomeAtlas.try_emplace(ICE_SPIKES, std::make_unique<IceSpikesBiome>(seed, std::pair(-1.0f, -0.8f), std::pair(0.7f, 0.9f)));
	_biomeAtlas.try_emplace(MOUNTAINS, std::make_unique<MountainsBiome>(seed, std::pair(-1.0f, 1.0f), std::pair(0.9f, 1.0f)));
}

const ABiome & BiomeManager::getBiome(BiomeType biomeType)
{
	BiomeAtlas::iterator it = _biomeAtlas.find(biomeType);
	if (it == _biomeAtlas.end())
		throw VoxelException("Couldn't get biome type: " + toString(biomeType) + ".");
	return *it->second;
}

const ABiome & BiomeManager::getBiomeAt(float temperature, float height)
{
	for (auto & [type, biome] : _biomeAtlas)
	{
		const std::pair<float, float> & temperatureRange = biome->getTemperatureRange();
		const std::pair<float, float> & heightRange = biome->getHeightRange();

		float temperatureMin = temperatureRange.first;
		float temperatureMax = temperatureRange.second;

		float temperatureDiff = getDistanceInterval(temperatureMin, temperatureMax, temperature);

		float heightMin = heightRange.first;
		float heightMax = heightRange.second;

		float heightDiff = getDistanceInterval(heightMin, heightMax, height);

		if (temperatureDiff == 0 && heightDiff == 0)
			return *biome;
	}
	Logger::log(VOXEL, WARNING, "Requested biome with invalid temperature/height values: " + toString(temperature) + " | " + toString(height) + ". Defaulting to OCEAN.");
	return *_biomeAtlas[OCEAN];
}

std::vector<BiomeDistanceInfo> BiomeManager::getBiomeSamples(float temperature, float height)
{
	std::vector<BiomeDistanceInfo> biomeDistanceInfos;

	for (auto & [type, biome] : _biomeAtlas)
	{
		const std::pair<float, float> & temperatureRange = biome->getTemperatureRange();
		const std::pair<float, float> & heightRange = biome->getHeightRange();

		float temperatureMin = temperatureRange.first;
		float temperatureMax = temperatureRange.second;

		float temperatureDiff = getDistanceInterval(temperatureMin, temperatureMax, temperature);

		float heightMin = heightRange.first;
		float heightMax = heightRange.second;

		float heightDiff = getDistanceInterval(heightMin, heightMax, height);

		float finalDiff = std::abs(std::max(temperatureDiff, heightDiff));
		if (finalDiff < 0.2)
			biomeDistanceInfos.push_back({ finalDiff, biome.get() });
	}
	return biomeDistanceInfos;
}
