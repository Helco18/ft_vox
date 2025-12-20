#include "Chunk.hpp"
#include "World.hpp"
#include "TextureAtlas.hpp"
#include "Logger.hpp"
#include <iostream>
#include <cstring>

Asset Chunk::_generateQuadMesh(float width, float height, float depth, int face)
{
    Asset asset;
    asset.vertices.reserve(4);
    asset.indices.reserve(6);

    Vertex v[4];

	v[0].texCoord = {0.0f, 0.0f};
	v[1].texCoord = {1.0f, 0.0f};
	v[2].texCoord = {1.0f, 1.0f};
	v[3].texCoord = {0.0f, 1.0f};

	//     Y
    //     ↑
    //     |
    //     |
    //     o──────→ X
    //    /
    //   /
    //  Z

	// Front face (NORTH)
	// y ↑
	//   |
	// 1 | v3 ───── v2
	//   |  |        |
	//   |  |        |
	//   |  |        |
	// 0 | v0 ───── v1
	//   +----------------→ x
	//      0        1

	// 	Indices :
	// 3 ── 2
	// |  ╱ |
	// | ╱  |
	// 0 ── 1


    switch (face)
    {
        case NORTH:
        {
            v[0].position = { 1 - depth, 0,      height };
            v[1].position = { 1 - depth, 0,      0 };
            v[2].position = { 1 - depth, width, 0 };
            v[3].position = { 1 - depth, width, height };
			v[0].normal = { 1, 0, 0 };
			v[1].normal = { 1, 0, 0 };
			v[2].normal = { 1, 0, 0 };
			v[3].normal = { 1, 0, 0 };
            break;
        }
        case SOUTH:
        {
            v[0].position = { depth, 0,      0 };
            v[1].position = { depth, 0,      height };
            v[2].position = { depth, width, height };
            v[3].position = { depth, width, 0 };
			v[3].texCoord = {0.0f, 1.0f};
			v[0].normal = { -1, 0, 0 };
			v[1].normal = { -1, 0, 0 };
			v[2].normal = { -1, 0, 0 };
			v[3].normal = { -1, 0, 0 };
            break;
        }
        case WEST:
        {
            v[0].position = { width, 0,      depth };
            v[1].position = { 0,     0,      depth };
            v[2].position = { 0,     height, depth };
            v[3].position = { width, height, depth };
			v[0].normal = { 0, 0, -1 };
			v[1].normal = { 0, 0, -1 };
			v[2].normal = { 0, 0, -1 };
			v[3].normal = { 0, 0, -1 };
            break;
        }
        case EAST:
        {
            v[0].position = { 0,      0,      1 - depth };
            v[1].position = { width, 0,       1 - depth };
            v[2].position = { width, height, 1 - depth };
			v[3].position = { 0,      height, 1 - depth };
			v[0].normal = { 0, 0, 1 };
			v[1].normal = { 0, 0, 1 };
			v[2].normal = { 0, 0, 1 };
			v[3].normal = { 0, 0, 1 };
            break;
        }
        case BOTTOM:
        {
            v[0].position = { 0,      depth, 0 };
            v[1].position = { width,  depth, 0 };
            v[2].position = { width,  depth, height };
            v[3].position = { 0,      depth, height };
			v[0].normal = { 0, -1, 0 };
			v[1].normal = { 0, -1, 0 };
			v[2].normal = { 0, -1, 0 };
			v[3].normal = { 0, -1, 0 };
			break;
		}
		case TOP:
		{
            v[0].position = { width, 1 - depth, 0 };
            v[1].position = { 0,     1 - depth, 0 };
            v[2].position = { 0,     1 - depth, height };
            v[3].position = { width, 1 - depth, height };
			v[0].normal = { 0, 1, 0 };
			v[1].normal = { 0, 1, 0 };
			v[2].normal = { 0, 1, 0 };
			v[3].normal = { 0, 1, 0 };
			break;
		}
    }

    asset.vertices.insert(asset.vertices.end(), { v[0], v[1], v[2], v[3] });
    asset.indices = {
        0, 1, 2,
        2, 3, 0
    };

    return asset;
}

glm::ivec3 Chunk::_sliceToWorld(int axis, int sliceIndex, int u, int v) {
    if (axis == 0)
        return glm::ivec3(sliceIndex, u, v);
    if (axis == 1)
        return glm::ivec3(u, sliceIndex, v);
    return glm::ivec3(u, v, sliceIndex);
}

uint8_t Chunk::_getNeighborBlock(const glm::ivec3 & pos, const glm::ivec3 & normal)
{
	Chunk * northChunk = _world->getChunk(_chunkLocation.x + 1, _chunkLocation.y, _chunkLocation.z);
	Chunk * southChunk = _world->getChunk(_chunkLocation.x - 1, _chunkLocation.y, _chunkLocation.z);
	Chunk * eastChunk = _world->getChunk(_chunkLocation.x, _chunkLocation.y, _chunkLocation.z + 1);
	Chunk * westChunk = _world->getChunk(_chunkLocation.x, _chunkLocation.y, _chunkLocation.z - 1);
	Chunk * topChunk = _world->getChunk(_chunkLocation.x, _chunkLocation.y + 1, _chunkLocation.z);
	Chunk * bottomChunk = _world->getChunk(_chunkLocation.x, _chunkLocation.y - 1, _chunkLocation.z);

    int x = pos.x + normal.x;
    int y = pos.y + normal.y;
    int z = pos.z + normal.z;

    if (x < 0 || x >= CHUNK_WIDTH)
    {
        if (normal.x == -1 && southChunk)
            return southChunk->getBlock(CHUNK_WIDTH - 1, y, z);
        else if (normal.x == 1 && northChunk)
            return northChunk->getBlock(0, y, z);
        else
            return 0;
    }
    if (y < 0 || y >= CHUNK_HEIGHT)
    {
        if (normal.y == -1 && bottomChunk)
            return bottomChunk->getBlock(x, CHUNK_HEIGHT - 1, z);
        else if (normal.y == 1 && topChunk)
            return topChunk->getBlock(x, 0, z);
        else
            return 0;
    }
    if (z < 0 || z >= CHUNK_LENGTH)
    {
        if (normal.z == -1 && westChunk)
            return westChunk->getBlock(x, y, CHUNK_LENGTH - 1);
        else if (normal.z == 1 && eastChunk)
            return eastChunk->getBlock(x, y, 0);
        else
            return 0;
    }
    return _blocks[x][y][z];
}

void Chunk::_emitBlocksFace(const glm::ivec3 & pos, int countBlockWidth, int countBlockHeight, int face)
{
    Asset quad = _generateQuadMesh(countBlockWidth, countBlockHeight, 0.0f, face);
    uint32_t verticesAdded = _asset.vertices.size();
    for (Vertex & vertex : quad.vertices)
    {
        Vertex tmp = vertex;
        tmp.position += glm::vec3(pos.x + _chunkLocation.x * CHUNK_WIDTH,
                                  pos.y + _chunkLocation.y * CHUNK_HEIGHT,
                                  pos.z + _chunkLocation.z * CHUNK_LENGTH);

        Texture * texture = TextureAtlas::getTexture(_blocks[pos.x][pos.y][pos.z] == 1 ? "assets/textures/blue_stone.png" : "assets/textures/stone.png");
        tmp.uvMin = texture->uvMin;
        tmp.uvMax = texture->uvMax;

        _asset.vertices.push_back(tmp);
    }
    for (uint32_t indice : quad.indices)
    {
        _asset.indices.push_back(indice + verticesAdded);
    }
}

void Chunk::_generateSliceMeshing(int axis, int sliceIndex)
{
    int uMax;
    int vMax;

	//     Y(Y)
    //     ↑
    //     |
    //     |
    //     o──────→ X(N)
    //    /
    //   /
    //  Z(E)

    if (axis == 0)
    {
        uMax = CHUNK_HEIGHT;
        vMax = CHUNK_LENGTH;
    }
    else if (axis == 1)
    {
        uMax = CHUNK_WIDTH; 
        vMax = CHUNK_LENGTH;
    }
    else 
    {
        uMax = CHUNK_WIDTH;
        vMax = CHUNK_HEIGHT;
    }

    bool processed[uMax][vMax][2];
    std::memset(processed, 0, sizeof(processed));

    for (int u = 0; u < uMax; ++u)
    {
        for (int v = 0; v < vMax; ++v)
        {
            glm::ivec3 pos = _sliceToWorld(axis, sliceIndex, u, v);
            uint8_t block = _blocks[pos.x][pos.y][pos.z];
            if (block == 0 || (processed[u][v][0] && processed[u][v][1]))
                continue;

            uint8_t forward = _getNeighborBlock(pos, (axis == 0 ? glm::ivec3(1, 0, 0) : (axis == 1 ? glm::ivec3(0, 1, 0) : glm::ivec3(0, 0, 1))));
            BlockData blockData = BlockData::getBlockData(forward);
            if (!blockData.isVisible() && !processed[u][v][0])
            {
                int uNext = u + 1;
                int width = 1;
                while (uNext < uMax)
                {
                    glm::ivec3 nextPos = _sliceToWorld(axis, sliceIndex, uNext, v);
                    uint8_t nextBlock = _blocks[nextPos.x][nextPos.y][nextPos.z];
                    uint8_t nextForward = _getNeighborBlock(nextPos, (axis == 0 ? glm::ivec3(1, 0, 0) : (axis == 1 ? glm::ivec3(0, 1, 0) : glm::ivec3(0, 0, 1))));
                    blockData = BlockData::getBlockData(nextForward);
                    if (nextBlock != block || !blockData.isVisible() || processed[uNext][v][0])
                        break;
                    width++;
                    uNext++;
                }
                _emitBlocksFace(pos, width, 1, (axis == 0 ? NORTH  : (axis == 1 ? TOP : EAST)));
                for (int i = u; i < u + width; ++i)
                    processed[i][v][0] = true;
            }
            
            uint8_t backward = _getNeighborBlock(pos, (axis == 0 ? glm::ivec3(-1, 0, 0) : (axis == 1 ? glm::ivec3(0, -1, 0) : glm::ivec3(0, 0, -1))));
            blockData = BlockData::getBlockData(backward);
            if (!blockData.isVisible() && !processed[u][v][1])
            {
                int uNext = u + 1;
                int width = 1;
                while (uNext < uMax)
                {
                    glm::ivec3 nextPos = _sliceToWorld(axis, sliceIndex, uNext, v);
                    uint8_t nextBlock = _blocks[nextPos.x][nextPos.y][nextPos.z];
                    uint8_t nextBackward = _getNeighborBlock(nextPos, (axis == 0 ? glm::ivec3(-1, 0, 0) : (axis == 1 ? glm::ivec3(0, -1, 0) : glm::ivec3(0, 0, -1))));
                    blockData = BlockData::getBlockData(nextBackward);
                    if (nextBlock != block || !blockData.isVisible() || processed[uNext][v][1])
                        break;
                    width++;
                    uNext++;
                }
                _emitBlocksFace(pos, width, 1, (axis == 0 ? SOUTH : (axis == 1 ? BOTTOM : WEST)));
                for (int j = u; j < u + width; ++j)
                    processed[j][v][1] = true;
            }
        }
    }
}

void Chunk::_generateGreedyMesh()
{
    for (int i = 0; i < 3; ++i)
    {
        for (int sliceIndex = 0; sliceIndex < (i == 0 ? CHUNK_WIDTH : (i == 1 ? CHUNK_HEIGHT : CHUNK_LENGTH)); ++sliceIndex)
        {
            _generateSliceMeshing(i, sliceIndex);
        }
    }
}
