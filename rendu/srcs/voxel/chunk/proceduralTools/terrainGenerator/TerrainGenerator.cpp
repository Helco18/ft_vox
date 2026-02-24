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

void TerrainGenerator::_fillWorld(int x, int y, int z, float slope, double worldY)
{
	if (worldY >= -3 && worldY <= -1)
		_chunk->_blocks[x][y][z] = BlockType::SAND;
	else if (worldY <= _height - 2 - (_height % 2))
		_chunk->_blocks[x][y][z] = BlockType::STONE;
	else
	{
		if (slope > 0.05f)
			_chunk->_blocks[x][y][z] = BlockType::STONE;
		else
			_chunk->_blocks[x][y][z] = BlockType::DIRT;
	}
}

void TerrainGenerator::_splitSkyFromSea(int x, int y, int z, double worldY)
{
	_chunk->_blocks[x][y][z] = (worldY) <= SEA_LEVEL ? BlockType::WATER : BlockType::AIR;
}

void TerrainGenerator::_paintSurface(int x, int y, int z, float slope, double worldY)
{
	if (slope > 0.05f)
		_chunk->_blocks[x][y][z] = BlockType::STONE;
	else
		_chunk->_blocks[x][y][z] = worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
}

void TerrainGenerator::_computeBlock(int x, int y, int z, double worldX, double worldZ)
{
	int worldY = (y + _worldYOffset);
	float slope = _heightMap.getSlope(x, z);

	// Bloc le plus haut du terrain lorsqu'il est au-dessus de Y=SEA_LEVEL
	if (worldY == _height && worldY >= SEA_LEVEL)
		_paintSurface(x, y, z, slope, worldY);
	// On dépasse la hauteur du noise
	else if (worldY > _height)
		_splitSkyFromSea(x, y, z, worldY);
	// On remplit l'intérieur du terrain
	else
		_fillWorld(x, y, z, slope, worldY);
	// On creuse les caves
	if (_chunk->_blocks[x][y][z] != BlockType::AIR && _chunk->_blocks[x][y][z] != BlockType::WATER)
		_addCave(x, y, z, worldX, worldY, worldZ, _height);
}

void TerrainGenerator::_computeTerrainHeight(int x, int z, int worldX, int worldZ)
{
	const ABiome & biome = BiomeManager::getBiome(BiomeType::MOUNTAINS);
	if (biome.isWithinRange(_chunkLocation.y))
		_height = biome.computeBiomeHeight(_heightMap, x, z, worldX, worldZ);
}

void TerrainGenerator::generateTerrain()
{
	if (_chunkLocation.y < -2 || _chunkLocation.y > 2)
		_height = 0;
	else
		_heightMap.computeHeight(_worldXOffset, _worldZOffset, _world->getNoise(), 2);
	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		double worldX = static_cast<double>(x + _worldXOffset);
		for (int z = 0; z < CHUNK_LENGTH; ++z)
		{
			double worldZ = static_cast<double>(z + _worldZOffset);
			_computeTerrainHeight(x, z, worldX, worldZ);
			for (int y = 0; y < CHUNK_HEIGHT; ++y)
				_computeBlock(x, y, z, worldX, worldZ);
		}
	}
}
