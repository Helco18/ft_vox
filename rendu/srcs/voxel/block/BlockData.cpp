#include "BlockData.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"

BlockData::BlockDataRegistry BlockData::_dataRegistry;

void BlockData::init()
{
	std::vector<std::string> dirtTextures(6, "resources/assets/textures/dirt.png");
	std::vector<std::string> grassTextures(6, "resources/assets/textures/grass_block_side.png");
	grassTextures[BlockFace::BOTTOM] = "resources/assets/textures/dirt.png";
	grassTextures[BlockFace::TOP] = "resources/assets/textures/grass_block_top.png";
	std::vector<std::string> waterTextures(6, "resources/assets/textures/water.png");
	std::vector<std::string> sandTextures(6, "resources/assets/textures/sand.png");
	std::vector<std::string> stoneTextures(6, "resources/assets/textures/stone.png");
	std::vector<std::string> iceTextures(6, "resources/assets/textures/ice.png");
	iceTextures[BlockFace::TOP] = "resources/assets/textures/ice_top.png";
	std::vector<std::string> snowTextures(6, "resources/assets/textures/snow_2.png");
	std::vector<std::string> sandstoneTextures(6, "resources/assets/textures/sand_stone.png");
	std::vector<std::string> redSandTextures(6, "resources/assets/textures/red_sand.png");
	std::vector<std::string> redStoneTextures(6, "resources/assets/textures/red_stone.png");
	std::vector<std::string> whiteStoneTextures(6, "resources/assets/textures/white_stone.png");
	std::vector<std::string> blackStoneTextures(6, "resources/assets/textures/black_stone.png");
	std::vector<std::string> whiteGravelTextures(6, "resources/assets/textures/white_gravel.png");
	std::vector<std::string> brownStoneTextures(6, "resources/assets/textures/brown_stone.png");
	std::vector<std::string> magmaStoneTextures(6, "resources/assets/textures/magma_stone.png");
	
	_dataRegistry.emplace(AIR, BlockData("air", false, false, false, {}));
	_dataRegistry.emplace(DIRT, BlockData("dirt", true, true, false, dirtTextures));
	_dataRegistry.emplace(GRASS, BlockData("grass", true, true, false, grassTextures));
	_dataRegistry.emplace(WATER, BlockData("water", true, false, true, waterTextures));
	_dataRegistry.emplace(SAND, BlockData("sand", true, true, false, sandTextures));
	_dataRegistry.emplace(STONE, BlockData("stone", true, true, false, stoneTextures));
	_dataRegistry.emplace(ICE, BlockData("ice", true, true, false, iceTextures));
	_dataRegistry.emplace(SNOW, BlockData("snow", true, true, false, snowTextures));
	_dataRegistry.emplace(SANDSTONE, BlockData("sandstone", true, true, false, sandstoneTextures));
	_dataRegistry.emplace(RED_SAND, BlockData("red_sand", true, true, false, redSandTextures));
	_dataRegistry.emplace(RED_STONE, BlockData("red_stone", true, true, false, redStoneTextures));
	_dataRegistry.emplace(WHITE_STONE, BlockData("white_stone", true, true, false, whiteStoneTextures));
	_dataRegistry.emplace(BLACK_STONE, BlockData("black_stone", true, true, false, blackStoneTextures));
	_dataRegistry.emplace(WHITE_GRAVEL, BlockData("white_gravel", true, true, false, whiteGravelTextures));
	_dataRegistry.emplace(BROWN_STONE, BlockData("brown_stone", true, true, false, brownStoneTextures));
	_dataRegistry.emplace(MAGMA_STONE, BlockData("magma_stone", true, true, false, magmaStoneTextures));
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

