#include "World.hpp"

static RayState rayInit(const glm::vec3 & pos, const glm::vec3 & rayDir, const glm::vec3 & deltaDist)
{
	glm::ivec3	step;
	glm::vec3	sideDist;
	RayState	state;
	glm::ivec3	block(glm::floor(pos));
	
	for (int i = 0; i < 3; ++i)
	{
		if (rayDir[i] < 0)
		{
			step[i] = -1;
			sideDist[i] = (pos[i] - block[i]) * deltaDist[i];
		}
		else
		{
			step[i] = 1;
			sideDist[i] = (block[i] + 1.0f - pos[i]) * deltaDist[i];
		}
	}
	state.o = step;
	state.ray = sideDist;
	state.delta = deltaDist;
	return state;
}

TargetedBlock World::_processRay(const glm::vec3 & pos, RayState & state, float maxDistance)
{
	float		distance = 0;
	glm::ivec3	block(glm::floor(pos));
	TargetedBlock		p { BlockFace::BOTTOM, glm::vec3(0.0f), BlockType::AIR };

	while (distance < maxDistance)
	{
		Chunk * chunk = getChunkAt(block.x, block.y, block.z);

		if (!chunk || chunk->getState() < BUILT)
			return p;
		glm::vec3 chunkpos = chunk->posToChunkPos(block);
		uint8_t blockType = chunk->getBlock(chunkpos.x, chunkpos.y, chunkpos.z);
		if (blockType != BlockType::AIR && blockType != BlockType::WATER)
		{
			p.pos = block;
			p.type = static_cast<BlockType>(blockType);
			return p;
		}

		if (state.ray.x < state.ray.y && state.ray.x < state.ray.z)
		{
			block.x += state.o.x;
			distance = state.ray.x;
			state.ray.x += state.delta.x;
			state.o.x > 0 ? p.face = NORTH : p.face = SOUTH;
		}
		else if (state.ray.y < state.ray.z)
		{
			block.y += state.o.y;
			distance = state.ray.y;
			state.ray.y += state.delta.y;
			state.o.x > 0 ? p.face = TOP : p.face = BOTTOM;
		}
		else
		{
			block.z += state.o.z;
			distance = state.ray.z;
			state.ray.z += state.delta.z;
			state.o.x > 0 ? p.face = EAST : p.face = WEST;
		}
	}
	return p;
}

TargetedBlock World::rayCast(const glm::vec3 & pos, const glm::vec3 & dir, float maxDistance)
{
	glm::vec3	rayDir(glm::normalize(dir));
	glm::vec3	deltaDist(glm::abs(1 / (rayDir.x)), glm::abs(1 / rayDir.y), glm::abs(1 / rayDir.z));

	RayState state = rayInit(pos, rayDir, deltaDist);

	return _processRay(pos, state, maxDistance);
}
