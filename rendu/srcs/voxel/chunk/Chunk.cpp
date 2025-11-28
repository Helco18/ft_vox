#include "Chunk.hpp"
#include "utils.hpp"

void Chunk::build()
{
	if (_state != NONE)
		return;

	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int y = 0; y < CHUNK_HEIGHT; ++y)
		{
			for (int z = 0; z < CHUNK_LENGTH; ++z)
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
		for (int y = 0; y < CHUNK_HEIGHT; ++y)
		{
			for (int z = 0; z < CHUNK_LENGTH; ++z)
			{
				uint32_t verticesAdded = _asset.vertices.size();
				if (_blocks[x][y][z] == 1)
				{
					for (Vertex vertex : model.getVertices())
					{
						Vertex tmp = vertex;
						tmp.position.x += x;
						tmp.position.y += y;
						tmp.position.z += z;
						_asset.vertices.push_back(tmp);
					}
					for (uint32_t indice : model.getIndices())
						_asset.indices.push_back(indice + verticesAdded);
				}
			}
		}
	}
	_state = MESHED;
}

void Chunk::upload(AEngine * engine)
{
	engine->upload(_asset);
	_state = UPLOADED;
}
