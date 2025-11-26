#include "Chunk.hpp"

void Chunk::build()
{
	if (_state != NONE)
		return;

	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int y = 0; y < CHUNK_WIDTH; ++y)
		{
			for (int z = 0; z < CHUNK_WIDTH; ++z)
			{
				if (y <= 64)
					_blocks[x][y][z] = 1;
				else
					_blocks[x][y][z] = 0;
			}
		}
	}
	_state = BUILT;
}

void Chunk::generateMesh()
{
	const OBJModel model = OBJModel::getModel(CUBE);

	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int y = 0; y < CHUNK_WIDTH; ++y)
		{
			for (int z = 0; z < CHUNK_WIDTH; ++z)
			{
				if (_blocks[x][y][z] == 1)
				{
					_vertices.insert(_vertices.end(), model.getVertices().begin(), model.getVertices().end());
					_indices.insert(_indices.end(), model.getIndices().begin(), model.getIndices().end());
				}
			}
		}
	}
	_state = MESHED;
}

void Chunk::upload()
{
	// coucou à nous du futur, j'espère que vous avez fini les autres classes avant :)
	_state = UPLOADED;
}

void Chunk::unload()
{
	// y'aura surement autre chose au-dessus
	_state = MESHED;
}
