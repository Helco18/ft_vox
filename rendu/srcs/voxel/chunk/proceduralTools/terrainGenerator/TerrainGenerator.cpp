#include "TerrainGenerator.hpp"
#include "SimplexNoise.hpp"

TerrainGenerator::TerrainGenerator(World * world, const glm::vec3 & chunkLocation): _world(world), _chunkLocation(chunkLocation), _heightMap(CHUNK_WIDTH, CHUNK_LENGTH)
{
	_worldXOffset = static_cast<double>(_chunkLocation.x * CHUNK_WIDTH);
	_worldYOffset = static_cast<double>(_chunkLocation.y * CHUNK_HEIGHT);
	_worldZOffset = static_cast<double>(_chunkLocation.z * CHUNK_LENGTH);
}

void TerrainGenerator::generateTerrain(Chunk * chunk)
{
	int height;
	if (_chunkLocation.y < -2 || _chunkLocation.y > 2)
		height = 0;
	else
		_heightMap.computeHeight(_worldXOffset, _worldZOffset, _world->getNoise(), 2);
	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		double worldX = static_cast<double>(x + _worldXOffset);
		for (int z = 0; z < CHUNK_LENGTH; ++z)
		{
			double worldZ = static_cast<double>(z + _worldZOffset);
			if (_chunkLocation.y < -2 || _chunkLocation.y > 2)
				height = 0;
			else
			{
				double noiseValue = _heightMap.getHeight(x, z);
				height = static_cast<int>(std::floor(noiseValue * 30));
			}
			for (int y = 0; y < CHUNK_HEIGHT; ++y)
			{
				int worldY = (y + _worldYOffset);
				float slope = _heightMap.getSlope(x, z);
				if (worldY == height && worldY >= 0)
				{
					if (slope > 0.05f)
						chunk->_blocks[x][y][z] = BlockType::STONE;
					else
						chunk->_blocks[x][y][z] = worldY <= 2 ? BlockType::SAND : BlockType::GRASS;
				}
				else if (worldY > height)
					chunk->_blocks[x][y][z] = (worldY) <= 0 ? BlockType::WATER : BlockType::AIR;
				else
				{
					if (worldY >= -3 && worldY <= -1)
						chunk->_blocks[x][y][z] = BlockType::SAND;
					else if (worldY <= height - 2 - (height % 2))
						chunk->_blocks[x][y][z] = BlockType::STONE;
					else
					{
						if (slope > 0.05f)
							chunk->_blocks[x][y][z] = BlockType::STONE;
						else
							chunk->_blocks[x][y][z] = BlockType::DIRT;
					}
				}
				if (chunk->_blocks[x][y][z] != BlockType::AIR && chunk->_blocks[x][y][z] != BlockType::WATER)
					_addCave(chunk, worldX, worldY, worldZ, x, y, z, height);
			}
		}
	}
}

void TerrainGenerator::_addCave(Chunk * chunk, double worldX, double worldY, double worldZ, int x, int y, int z, int height)
{
	double depthFactor = std::clamp((-worldY +20) / 50.0, 0.1, 1.0);
	// --- CAVE GENERATION --- : spaghetti 2
	double scaleX = 3.0;
	double scaleY = 5.0;
	double scaleZ = 3.0;

	double px = worldX * scaleX;
	double py = worldY * scaleY;
	double pz = static_cast<double>(worldZ) * scaleZ;
	const SimplexNoise<3> & noise = chunk->_world->getNoiseCave();

	double caveValue = noise.queryState({px, py, pz});

	double target = 0.0;     // millieu de la bande
	double epsilon = 0.032;    // largeur de la bande
	double gradientThreshold = 0.00035;  // clap de la cocille

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
			chunk->_blocks[x][y][z] = BlockType::AIR;
	}
	// --- CAVE GENERATION --- : cheese
	if (worldY < height - 60)
	{
		caveValue = chunk->_world->getNoiseCave().queryState({worldX * 2.0, worldY * 4.0, static_cast<double>(worldZ) * 2.0});
		depthFactor = std::clamp(-worldY / 50.0, 0.0, 1.0);
		caveValue *= depthFactor;
		if (caveValue > 0.3)
			chunk->_blocks[x][y][z] = BlockType::AIR;
	}
}
