#include "Chunk.hpp"
#include "TextureAtlas.hpp"
#include "Logger.hpp"
#include "BlockData.hpp"
#include "Profiler.hpp"

ChunkAsset Chunk::_generateQuadMesh(float width, float height, float depth, int face)
{
	ChunkAsset asset;
	asset.vertices.reserve(4);
	asset.indices.reserve(6);

	ChunkVertex v[4];

	v[0].texCoord = {0.0f, 0.005f};
	v[1].texCoord = {1.0f, 0.005f};
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
			v[0].position = { 1 - depth, 0, width };
			v[1].position = { 1 - depth, 0, 0 };
			v[2].position = { 1 - depth, height, 0 };
			v[3].position = { 1 - depth, height, width };
			v[0].normal = { 1, 0, 0 };
			v[1].normal = { 1, 0, 0 };
			v[2].normal = { 1, 0, 0 };
			v[3].normal = { 1, 0, 0 };
			break;
		}
		case SOUTH:
		{
			v[0].position = { depth, 0, 0 };
			v[1].position = { depth, 0, width };
			v[2].position = { depth, height, width };
			v[3].position = { depth, height, 0 };
			v[0].normal = { -1, 0, 0 };
			v[1].normal = { -1, 0, 0 };
			v[2].normal = { -1, 0, 0 };
			v[3].normal = { -1, 0, 0 };
			break;
		}
		case WEST:
		{
			v[0].position = { width, 0, depth };
			v[1].position = { 0,0, depth };
			v[2].position = { 0,height, depth };
			v[3].position = { width, height, depth };
			v[0].normal = { 0, 0, -1 };
			v[1].normal = { 0, 0, -1 };
			v[2].normal = { 0, 0, -1 };
			v[3].normal = { 0, 0, -1 };
			break;
		}
		case EAST:
		{
			v[0].position = { 0, 0, 1 - depth };
			v[1].position = { width, 0,  1 - depth };
			v[2].position = { width, height, 1 - depth };
			v[3].position = { 0, height, 1 - depth };
			v[0].normal = { 0, 0, 1 };
			v[1].normal = { 0, 0, 1 };
			v[2].normal = { 0, 0, 1 };
			v[3].normal = { 0, 0, 1 };
			break;
		}
		case BOTTOM:
		{
			v[0].position = { 0, depth, 0 };
			v[1].position = { width,  depth, 0 };
			v[2].position = { width,  depth, height };
			v[3].position = { 0, depth, height };
			v[0].normal = { 0, -1, 0 };
			v[1].normal = { 0, -1, 0 };
			v[2].normal = { 0, -1, 0 };
			v[3].normal = { 0, -1, 0 };
			break;
		}
		case TOP:
		{
			v[0].position = { width, 1 - depth, 0 };
			v[1].position = { 0,1 - depth, 0 };
			v[2].position = { 0,1 - depth, height };
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

inline glm::ivec3 Chunk::_sliceToWorld(int axis, int sliceIndex, int u, int v)
{
	if (axis == 0)
		return glm::ivec3(sliceIndex, u, v);
	if (axis == 1)
		return glm::ivec3(u, sliceIndex, v);
	return glm::ivec3(u, v, sliceIndex);
}

uint8_t Chunk::_getNeighborBlock(const glm::ivec3 & pos, const glm::ivec3 & normal)
{
	int x = pos.x + normal.x;
	int y = pos.y + normal.y;
	int z = pos.z + normal.z;

	if (x < 0 || x >= CHUNK_WIDTH)
	{
		if (normal.x == -1 && _southChunk && _southChunk->getState() >= BUILT)
			return _southChunk->getBlock(CHUNK_WIDTH - 1, y, z);
		else if (normal.x == 1 && _northChunk && _northChunk->getState() >= BUILT)
			return _northChunk->getBlock(0, y, z);
		else
			return 0;
	}
	if (y < 0 || y >= CHUNK_HEIGHT)
	{
		if (normal.y == -1 && _bottomChunk && _bottomChunk->getState() >= BUILT)
			return _bottomChunk->getBlock(x, CHUNK_HEIGHT - 1, z);
		else if (normal.y == 1 && _topChunk && _topChunk->getState() >= BUILT)
			return _topChunk->getBlock(x, 0, z);
		else
			return 0;
	}
	if (z < 0 || z >= CHUNK_LENGTH)
	{
		if (normal.z == -1 && _westChunk && _westChunk->getState() >= BUILT)
			return _westChunk->getBlock(x, y, CHUNK_LENGTH - 1);
		else if (normal.z == 1 && _eastChunk && _eastChunk->getState() >= BUILT)
			return _eastChunk->getBlock(x, y, 0);
		else
			return 0;
	}
	return _blocks[x][y][z];
}

void Chunk::_emitBlocksFace(const glm::ivec3 & pos, int countBlockWidth, int countBlockHeight, int face)
{
	ChunkAsset quad;
	if (_blocks[pos.x][pos.y][pos.z] == BlockType::WATER && face == TOP)
		quad = _generateQuadMesh(countBlockWidth, countBlockHeight, 0.2f, face);
	else
		quad = _generateQuadMesh(countBlockWidth, countBlockHeight, 0.0f, face);
	uint32_t verticesAddedO = _chunkOpaqueAsset.vertices.size();
	uint32_t verticesAddedT = _chunkTransparencyAsset.vertices.size();
	for (ChunkVertex & vertex : quad.vertices)
	{
		ChunkVertex tmp = vertex;
		tmp.position += glm::vec3(pos.x + _chunkLocation.x * CHUNK_WIDTH,
								  pos.y + _chunkLocation.y * CHUNK_HEIGHT,
								  pos.z + _chunkLocation.z * CHUNK_LENGTH);

		const std::string & texPath = BlockData::getBlockData(_blocks[pos.x][pos.y][pos.z]).getTexturePath(face);
		Texture * texture = TextureAtlas::getTexture(texPath);
		if (!texture)
			return;
		if (_blocks[pos.x][pos.y][pos.z] == BlockType::WATER)
			tmp.alpha = 0.5f;
		else
			tmp.alpha = 1.0f;
		tmp.uvMin = texture->uvMin;
		tmp.uvMax = texture->uvMax;
		tmp.uvRepeat = { static_cast<float>(countBlockWidth), static_cast<float>(countBlockHeight) };

		if (_blocks[pos.x][pos.y][pos.z] == BlockType::WATER)
			_chunkTransparencyAsset.vertices.push_back(tmp);
		else
			_chunkOpaqueAsset.vertices.push_back(tmp);
	}
	if (_blocks[pos.x][pos.y][pos.z] == BlockType::WATER)
	{
		for (uint32_t indice : quad.indices)
			_chunkTransparencyAsset.indices.push_back(indice + verticesAddedT);
	}
	else
	{
		for (uint32_t indice : quad.indices)
			_chunkOpaqueAsset.indices.push_back(indice + verticesAddedO);
	}
}

static glm::ivec3 getFaceDir(int axis, FaceDirection faceDir)
{
	switch (axis)
	{
		case 0: return (faceDir == FaceDirection::FORWARD ? glm::ivec3(1,0,0) : glm::ivec3(-1,0,0));
		case 1: return (faceDir == FaceDirection::FORWARD ? glm::ivec3(0,1,0) : glm::ivec3(0,-1,0));
		case 2: return (faceDir == FaceDirection::FORWARD ? glm::ivec3(0,0,1) : glm::ivec3(0,0,-1));
	}
	return glm::ivec3(0);
}

static BlockFace getBlockFace(int axis, FaceDirection faceDir)
{
	switch (axis)
	{
		case 0: return (faceDir == FaceDirection::FORWARD ? NORTH : SOUTH);
		case 1: return (faceDir == FaceDirection::FORWARD ? TOP : BOTTOM);
		case 2: return (faceDir == FaceDirection::FORWARD ? EAST : WEST);
	}
	return NORTH;
}

static int getProcessedIndex(FaceDirection faceDir)
{
	return (faceDir == FaceDirection::FORWARD ? 0 : 1);
}

void Chunk::_processFace(int u, int v, std::vector<std::array<bool,2>> & processed, FaceDirection faceDir, int axis, int sliceIndex, int uMax, int vMax)
{
	int pIndex = getProcessedIndex(faceDir);
	glm::ivec3 dir = getFaceDir(axis, faceDir);
	BlockFace face = getBlockFace(axis, faceDir);

	glm::ivec3 pos = _sliceToWorld(axis, sliceIndex, u, v);
	uint8_t block = _blocks[pos.x][pos.y][pos.z];
	if (block == 0 || processed[u + v * uMax][pIndex])
		return;

	uint8_t neighbor = _getNeighborBlock(pos, dir);
	BlockData blockData = BlockData::getBlockData(neighbor);

	if (blockData.isVisible() && (!blockData.isLiquid() || (blockData.isLiquid() && (block == neighbor))))
		return;

	int width = 1;
	int uNext = u + 1;
	while (uNext < uMax)
	{
		glm::ivec3 nextPos = _sliceToWorld(axis, sliceIndex, uNext, v);
		uint8_t nextBlock = _blocks[nextPos.x][nextPos.y][nextPos.z];
		uint8_t nextNeighbor = _getNeighborBlock(nextPos, dir);
		blockData = BlockData::getBlockData(nextNeighbor);
		if (nextBlock != block || (blockData.isVisible() && (!blockData.isLiquid() || (blockData.isLiquid() && (nextBlock != neighbor)))) || processed[uNext + v * uMax][pIndex])
			break;
		width++;
		uNext++;
	}

	int height = 1;
	int oldHeight = 0;
	for (int i = u; i < u + width; ++i)
	{
		int vNext = v + 1;
		height = 1;
		while (vNext < vMax)
		{
			glm::ivec3 nextPos = _sliceToWorld(axis, sliceIndex, i, vNext);
			uint8_t nextBlock = _blocks[nextPos.x][nextPos.y][nextPos.z];
			uint8_t nextNeighbor = _getNeighborBlock(nextPos, dir);
			blockData = BlockData::getBlockData(nextNeighbor);
			if (nextBlock != block || (blockData.isVisible() && (!blockData.isLiquid() || (blockData.isLiquid() && (nextBlock != neighbor)))) || processed[i + vNext * uMax][pIndex])
				break;
			height++;
			vNext++;
		}
		if (oldHeight == 0 || height < oldHeight)
			oldHeight = height;
	}
	if (oldHeight != 0)
		height = oldHeight;

	if (axis == 0)
		_emitBlocksFace(pos, height, width, face);
	else
		_emitBlocksFace(pos, width, height, face);

	for (int i = u; i < u + width; ++i)
		for (int j = v; j < v + height; ++j)
			processed[i + j * uMax][pIndex] = true;
}

void Chunk::_generateSliceMeshing(int axis, int sliceIndex)
{
	int uMax;
	int vMax;
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

	std::vector<std::array<bool,2>> processed(uMax * vMax, {false, false});

	for (int u = 0; u < uMax; ++u)
	{
		for (int v = 0; v < vMax; ++v)
		{
			_processFace(u, v, processed, FaceDirection::FORWARD, axis, sliceIndex, uMax, vMax);
			_processFace(u, v, processed, FaceDirection::BACKWARD, axis, sliceIndex, uMax, vMax);
		}
	}
}

void Chunk::_generateFrameMesh()
{
	if (!_asset.vertices.data)
		return;
	glm::vec3 pos[24];

	pos[0] = _chunkLocation;
	pos[1] = {_chunkLocation.x, _chunkLocation.y + 1, _chunkLocation.z};

	pos[2] = _chunkLocation;
	pos[3] = {_chunkLocation.x, _chunkLocation.y, _chunkLocation.z + 1};

	pos[4] = _chunkLocation;
	pos[5] = {_chunkLocation.x + 1, _chunkLocation.y, _chunkLocation.z};


	pos[6] = {_chunkLocation.x + 1, _chunkLocation.y + 1, _chunkLocation.z + 1};
	pos[7] = {_chunkLocation.x + 1, _chunkLocation.y, _chunkLocation.z + 1};

	pos[8] = {_chunkLocation.x + 1, _chunkLocation.y + 1, _chunkLocation.z + 1};
	pos[9] = {_chunkLocation.x + 1, _chunkLocation.y + 1, _chunkLocation.z};

	pos[10] = {_chunkLocation.x + 1, _chunkLocation.y + 1, _chunkLocation.z + 1};
	pos[11] = {_chunkLocation.x, _chunkLocation.y + 1, _chunkLocation.z + 1};

//=========================================

	pos[12] = {_chunkLocation.x, _chunkLocation.y + 1, _chunkLocation.z};
	pos[13] = {_chunkLocation.x, _chunkLocation.y + 1, _chunkLocation.z + 1};

	pos[14] = {_chunkLocation.x, _chunkLocation.y + 1, _chunkLocation.z};
	pos[15] = {_chunkLocation.x + 1, _chunkLocation.y + 1, _chunkLocation.z};


	pos[16] = {_chunkLocation.x + 1, _chunkLocation.y, _chunkLocation.z + 1};
	pos[17] = {_chunkLocation.x, _chunkLocation.y, _chunkLocation.z + 1};

	pos[18] = {_chunkLocation.x + 1, _chunkLocation.y, _chunkLocation.z + 1};
	pos[19] = {_chunkLocation.x + 1, _chunkLocation.y, _chunkLocation.z};


	pos[20] = {_chunkLocation.x + 1, _chunkLocation.y, _chunkLocation.z};
	pos[21] = {_chunkLocation.x + 1, _chunkLocation.y + 1, _chunkLocation.z};

	pos[22] = {_chunkLocation.x, _chunkLocation.y, _chunkLocation.z + 1};
	pos[23] = {_chunkLocation.x, _chunkLocation.y + 1, _chunkLocation.z + 1};

	for (int i = 0; i < 24; ++i)
	{
		pos[i].x *= CHUNK_WIDTH;
		pos[i].y *= CHUNK_HEIGHT;
		pos[i].z *= CHUNK_LENGTH;

		_linesPos.push_back(pos[i]);
	}
	_assetFrame.vertices.data = _linesPos.data();
	_assetFrame.vertices.vertexCount = _linesPos.size();
	_assetFrame.vertices.size = _linesPos.size() * sizeof(glm::vec3);
	_assetFrame.vertices.stride = sizeof(glm::vec3);
}

void Chunk::_generateGreedyMesh()
{
	Profiler p("Chunk::generateGreedyMesh");
	_chunkOpaqueAsset.vertices.clear();
	_chunkOpaqueAsset.indices.clear();
	_chunkTransparencyAsset.vertices.clear();
	_chunkTransparencyAsset.indices.clear();
	_chunkFinalAsset.vertices.clear();
	_chunkFinalAsset.indices.clear();

	for (int i = 0; i < 3; ++i)
	{
		for (int sliceIndex = 0; sliceIndex < (i == 0 ? CHUNK_WIDTH : (i == 1 ? CHUNK_HEIGHT : CHUNK_LENGTH)); ++sliceIndex)
			_generateSliceMeshing(i, sliceIndex);
	}
	if ((!_chunkOpaqueAsset.vertices.empty() && !_chunkOpaqueAsset.indices.empty()) || (!_chunkTransparencyAsset.vertices.empty() && !_chunkTransparencyAsset.indices.empty()))
	{
		for (size_t i = 0; i < _chunkTransparencyAsset.indices.size() ; ++i)
			_chunkTransparencyAsset.indices[i] += _chunkOpaqueAsset.vertices.size();

		_chunkFinalAsset.indices.insert(_chunkFinalAsset.indices.end(), _chunkOpaqueAsset.indices.begin(), _chunkOpaqueAsset.indices.end());
		_chunkFinalAsset.vertices.insert(_chunkFinalAsset.vertices.end(), _chunkOpaqueAsset.vertices.begin(), _chunkOpaqueAsset.vertices.end());
		_chunkFinalAsset.indices.insert(_chunkFinalAsset.indices.end(), _chunkTransparencyAsset.indices.begin(), _chunkTransparencyAsset.indices.end());
		_chunkFinalAsset.vertices.insert(_chunkFinalAsset.vertices.end(), _chunkTransparencyAsset.vertices.begin(), _chunkTransparencyAsset.vertices.end());
		
		_asset.vertices.data = _chunkFinalAsset.vertices.data();
		_asset.vertices.vertexCount = _chunkFinalAsset.vertices.size();
		_asset.indices = _chunkFinalAsset.indices;
		_asset.vertices.size = _chunkFinalAsset.vertices.size() * sizeof(ChunkVertex);
		_chunkTransparencyAsset.indices.clear();
		_chunkOpaqueAsset.indices.clear();
		_chunkFinalAsset.indices.clear();
	}
	_generateFrameMesh();
	_asset.vertices.stride = sizeof(ChunkVertex);
}
