#include "BlockData.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"

BlockData::BlockDataRegistry BlockData::_dataRegistry;

void BlockData::init()
{
	std::vector<std::string> dirtTextures(6, "resources/assets/textures/dirt.png");
	std::vector<std::string> grassTextures(6, "resources/assets/textures/dirt_tmp.png");
	grassTextures[BlockFace::BOTTOM] = "resources/assets/textures/dirt.png";
	grassTextures[BlockFace::TOP] = "resources/assets/textures/grass_block_top.png";
	std::vector<std::string> waterTextures(6, "resources/assets/textures/blue_stone.png");
	std::vector<std::string> sandTextures(6, "resources/assets/textures/sand.png");
	std::vector<std::string> stoneTextures(6, "resources/assets/textures/stone.png");
	
	_dataRegistry.emplace(AIR, BlockData("air", false, false, false, {}));
	_dataRegistry.emplace(DIRT, BlockData("dirt", true, true, false, dirtTextures));
	_dataRegistry.emplace(GRASS, BlockData("grass", true, true, false, grassTextures));
	_dataRegistry.emplace(WATER, BlockData("water", true, false, true, waterTextures));
	_dataRegistry.emplace(SAND, BlockData("sand", true, true, false, sandTextures));
	_dataRegistry.emplace(STONE, BlockData("stone", true, true, false, stoneTextures));
}

BlockData & BlockData::getBlockData(uint8_t type)
{
	BlockDataRegistry::iterator it = _dataRegistry.find(type);
	if (it != _dataRegistry.end())
		return it->second;
	throw VoxelException("Invalid BlockData #" + toString(static_cast<int>(type)) + " requested.");
}

const std::string & BlockData::getTexturePath(int blockFace)
{
	return _textures[blockFace];
}

