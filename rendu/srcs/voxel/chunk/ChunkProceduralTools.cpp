#include "Chunk.hpp"
#include "SimplexNoise.hpp"
#include "World.hpp"

void Chunk::_addCave(double worldX, double worldY, double worldZ, int x, int y, int z, int height)
{
	double depthFactor = std::clamp((-worldY +20) / 50.0, 0.1, 1.0);
	// --- CAVE GENERATION --- : spaghetti 2
	double scaleX = 3.0;
	double scaleY = 5.0;
	double scaleZ = 3.0;

	double px = worldX * scaleX;
	double py = worldY * scaleY;
	double pz = static_cast<double>(worldZ) * scaleZ;

	SimplexNoise<3> noise = _world->getNoiseCave();

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
			_blocks[x][y][z] = BlockType::AIR;
	}
	// --- CAVE GENERATION --- : cheese
	if (worldY < height - 60)
	{
		caveValue = _world->getNoiseCave().queryState({worldX * 2.0, worldY * 4.0, static_cast<double>(worldZ) * 2.0});
		depthFactor = std::clamp(-worldY / 50.0, 0.0, 1.0);
		caveValue *= depthFactor;
		if (caveValue > 0.3)
			_blocks[x][y][z] = BlockType::AIR;
	}
}
