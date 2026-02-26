#include "TerrainGenerator.hpp"
#include "BiomeManager.hpp"
#include "SimplexNoise.hpp"

TerrainGenerator::TerrainGenerator(Chunk * chunk, World * world, const glm::vec3 & chunkLocation):
	_chunk(chunk), _world(world), _chunkLocation(chunkLocation), _heightMap(CHUNK_WIDTH, CHUNK_LENGTH), _biomeMap(CHUNK_WIDTH, CHUNK_LENGTH)
{
	_worldXOffset = static_cast<double>(_chunkLocation.x * CHUNK_WIDTH);
	_worldYOffset = static_cast<double>(_chunkLocation.y * CHUNK_HEIGHT);
	_worldZOffset = static_cast<double>(_chunkLocation.z * CHUNK_LENGTH);
}

void TerrainGenerator::_addCave(int x, int y, int z, int worldX, int worldY, int worldZ, int height)
{
	double depthFactor = std::clamp((-worldY +20) / 50.0, 0.1, 1.0);
	// --- CAVE GENERATION --- : spaghetti 2
	double scaleX = 3.0;
	double scaleY = 5.0;
	double scaleZ = 3.0;

	double px = worldX * scaleX;
	double py = worldY * scaleY;
	double pz = static_cast<double>(worldZ) * scaleZ;
	const SimplexNoise<3> & noise = _chunk->_world->getNoiseCave();

	double caveValue = noise.queryState({px, py, pz});

	double target = 0.0;     // milieu de la bande
	double epsilon = 0.032;    // largeur de la bande
	double gradientThreshold = 0.00035;  // clap de la coquille

	// Logger::log(VOXEL, INFO, "Cave value: " + toString(caveValue) + ", Gradient magnitude: " + toString(gradientMagnitude));
	if (std::abs(caveValue - target) < epsilon)
	{
		double h = 0.08; // pas du gradient

		double nx1 = noise.queryState({px + h, py, pz});
		double ny1 = noise.queryState({px, py + h, pz});
		double nz1 = noise.queryState({px, py, pz + h});

		double gx = nx1 - caveValue;
		double gy = ny1 - caveValue;
		double gz = nz1 - caveValue;
		double gradientMagnitude = sqrt(gx * gx + gy * gy + gz * gz);
		if (gradientMagnitude / depthFactor < gradientThreshold)
			_chunk->_blocks[x][y][z] = BlockType::AIR;
	}
	// --- CAVE GENERATION --- : cheese
	if (worldY < height - 60)
	{
		caveValue = _chunk->_world->getNoiseCave().queryState({worldX * 2.0, worldY * 4.0, static_cast<double>(worldZ) * 2.0});
		depthFactor = std::clamp(-worldY / 50.0, 0.0, 1.0);
		caveValue *= depthFactor;
		if (caveValue > 0.3)
			_chunk->_blocks[x][y][z] = BlockType::AIR;
	}
}

void TerrainGenerator::_addFlyingIsland(int x, int y, int z, int worldX, int worldY, int worldZ, int )
{
	double hight = _world->getTerrainNoise().queryState({static_cast<double>(worldX), static_cast<double>(worldZ)});
	double start = 192;
	double end = 191;
	if (hight < -0.3)
	{
		start = 640 - abs(hight) * 40;
		end = 640 - 14 + abs(hight) * 10;
	}
	else
		return ;
	if (worldY >= start && worldY <= end)
	{
		if (worldY == static_cast<int>(end))
			_chunk->_blocks[x][y][z] = BlockType::GRASS;
		else if (worldY <= end - 2 - (static_cast<int>(end) % 2))
			_chunk->_blocks[x][y][z] = BlockType::STONE;
		else
			_chunk->_blocks[x][y][z] = BlockType::DIRT;
	}
}

uint8_t TerrainGenerator::_computeBlock(const ABiome & biome, BiomePaintingInfo & biomePaintingInfo)
{
	int worldY = biomePaintingInfo.worldY;
	int height = biomePaintingInfo.heightMap->getHeight(biomePaintingInfo.x, biomePaintingInfo.z);
	// Bloc le plus haut du terrain lorsqu'il est au-dessus de Y=SEA_LEVEL
	if (worldY == height && worldY >= SEA_LEVEL)
		return biome.paintSurface(biomePaintingInfo);
	// On dépasse la hauteur du noise
	else if (worldY > height)
		return biome.splitSkyFromSea(biomePaintingInfo);
	// On remplit l'intérieur du terrain
	else
		return biome.fillWorld(biomePaintingInfo);
}

double TerrainGenerator::_computeTerrainHeight(const BiomePaintingInfo & paintingInfo)
{
	std::vector<BiomeDistanceInfo> biomeDistanceInfos = BiomeManager::getBiomeSamples(paintingInfo.temperature,
		(std::clamp(_biomeMap.getHeight(paintingInfo.x, paintingInfo.z) * 0.5, -1.0, 1.0)));
	std::vector<double> biomeHeights;

	for (BiomeDistanceInfo & biomeDistanceInfo : biomeDistanceInfos)
	{
		ABiome * biome = biomeDistanceInfo.biome;
		double biomeHeight = biome->computeBiomeHeight(paintingInfo);

		while (biomeDistanceInfo.distance < 0.1f)
		{
			biomeHeights.push_back(biomeHeight);
			biomeDistanceInfo.distance += 0.001f;
		}
	}

	if (biomeHeights.empty())
		return 0;
	return std::accumulate(biomeHeights.begin(), biomeHeights.end(), 0.0f) / biomeHeights.size();
}

void TerrainGenerator::generateTerrain()
{
	BiomePaintingInfo paintingInfo;
	paintingInfo.heightMap = &_heightMap;
	paintingInfo.biomeMap = &_biomeMap;

	_biomeMap.computeHeight(_worldXOffset, _worldZOffset, _world->getHeightNoise(), 2);
	if (_chunkLocation.y >= -2 && _chunkLocation.y <= 2)
		_heightMap.computeHeight(_worldXOffset, _worldZOffset, _world->getTerrainNoise(), 2);
	for (int x = -1; x < CHUNK_WIDTH + 1; ++x)
	{
		paintingInfo.x = x;
		paintingInfo.worldX = x + _worldXOffset;
		for (int z = -1; z < CHUNK_LENGTH + 1; ++z)
		{
			paintingInfo.z = z;
			paintingInfo.worldZ = z + _worldZOffset;
			_heightMap.setHeight(x, z, _computeTerrainHeight(paintingInfo));
		}
	}
	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		int worldX = x + _worldXOffset;
		paintingInfo.x = x;
		paintingInfo.worldX = worldX;
		for (int z = 0; z < CHUNK_LENGTH; ++z)
		{
			int worldZ = z + _worldZOffset;
			paintingInfo.z = z;
			paintingInfo.worldZ = worldZ;
			paintingInfo.temperature = std::clamp((worldX * 0.0001) + _world->getTemperatureNoise().queryState({static_cast<double>(worldX), static_cast<double>(worldZ)}) * 0.02, -1.0, 1.0);
			const ABiome & biome = BiomeManager::getBiomeAt(paintingInfo.temperature, std::clamp(_biomeMap.getHeight(x, z) * 0.5, -1.0, 1.0));
			paintingInfo.slope = _heightMap.getSlope(x, z);
			for (int y = 0; y < CHUNK_HEIGHT; ++y)
			{
				int height = _heightMap.getHeight(x, z);
				int worldY = (y + _worldYOffset);
				paintingInfo.worldY = worldY;
				_chunk->_blocks[x][y][z] = _computeBlock(biome, paintingInfo);
				// On creuse les caves
				if (_chunk->_blocks[x][y][z] != BlockType::AIR && _chunk->_blocks[x][y][z] != BlockType::WATER)
					_addCave(x, y, z, worldX, worldY, worldZ, height);
				if (_chunkLocation.y >= 18 && _chunkLocation.y <= 20)
					_addFlyingIsland(x, y, z, worldX, worldY, worldZ, height);
			}
		}
	}
}
