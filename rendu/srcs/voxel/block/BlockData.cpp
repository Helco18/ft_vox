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
	
	_dataRegistry.emplace(AIR, BlockData("air", false, false, {}));
	_dataRegistry.emplace(DIRT, BlockData("dirt", true, false, dirtTextures));
	_dataRegistry.emplace(GRASS, BlockData("grass", true, false, grassTextures));
	_dataRegistry.emplace(WATER, BlockData("water", true, true, waterTextures));
}

BlockData & BlockData::getBlockData(uint8_t type)
{
	BlockDataRegistry::iterator it = _dataRegistry.find(type);
	if (it != _dataRegistry.end())
		return it->second;
	throw VoxelException("Invalid BlockData #" + toString(type) + " requested.");
}

const std::string & BlockData::getTexturePath(int blockFace)
{
	return _textures[blockFace];
}

