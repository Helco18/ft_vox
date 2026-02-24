#include "TerrainGenerator.hpp"
#include "BiomeManager.hpp"
#include "SimplexNoise.hpp"

TerrainGenerator::TerrainGenerator(Chunk * chunk, World * world, const glm::vec3 & chunkLocation):
	_chunk(chunk), _world(world), _chunkLocation(chunkLocation), _heightMap(CHUNK_WIDTH, CHUNK_LENGTH)
{
	_worldXOffset = static_cast<double>(_chunkLocation.x * CHUNK_WIDTH);
	_worldYOffset = static_cast<double>(_chunkLocation.y * CHUNK_HEIGHT);
	_worldZOffset = static_cast<double>(_chunkLocation.z * CHUNK_LENGTH);
}

void TerrainGenerator::_addCave(int x, int y, int z, double worldX, double worldY, double worldZ, int height)
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

uint8_t TerrainGenerator::_computeBlock(const ABiome & biome, float slope, double worldY, int height)
{
	// Bloc le plus haut du terrain lorsqu'il est au-dessus de Y=SEA_LEVEL
	if (worldY == height && worldY >= SEA_LEVEL)
		return biome.paintSurface(worldY, slope);
	// On dépasse la hauteur du noise
	else if (worldY > height)
		return biome.splitSkyFromSea(worldY);
	// On remplit l'intérieur du terrain
	else
		return biome.fillWorld(height, worldY, slope);
}

int TerrainGenerator::_computeTerrainHeight(const ABiome & biome, int x, int z, int worldX, int worldZ)
{
	if (biome.isWithinRange(_chunkLocation.y))
		return biome.computeBiomeHeight(_heightMap, x, z, worldX, worldZ);
	return 0;
}

void TerrainGenerator::generateTerrain()
{
	if (_chunkLocation.y > -2 && _chunkLocation.y < 2)
		_heightMap.computeHeight(_worldXOffset, _worldZOffset, _world->getNoise(), 2);
	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		double worldX = static_cast<double>(x + _worldXOffset);
		for (int z = 0; z < CHUNK_LENGTH; ++z)
		{
			const ABiome & biome = BiomeManager::getBiome(BiomeType::PLAINS);
			double worldZ = static_cast<double>(z + _worldZOffset);
			_heightMap.setHeight(x, z, _computeTerrainHeight(biome, x, z, worldX, worldZ));
		}
	}
	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		double worldX = static_cast<double>(x + _worldXOffset);
		for (int z = 0; z < CHUNK_LENGTH; ++z)
		{
			const ABiome & biome = BiomeManager::getBiome(BiomeType::PLAINS);
			double worldZ = static_cast<double>(z + _worldZOffset);
			float slope = _heightMap.getSlope(x, z);
			for (int y = 0; y < CHUNK_HEIGHT; ++y)
			{
				int height = _heightMap.getHeight(x, z);
				double worldY = (y + _worldYOffset);
				_chunk->_blocks[x][y][z] = _computeBlock(biome, slope, worldY, height);
				// On creuse les caves
				if (_chunk->_blocks[x][y][z] != BlockType::AIR && _chunk->_blocks[x][y][z] != BlockType::WATER)
					_addCave(x, y, z, worldX, worldY, worldZ, height);
			}
		}
	}
}
