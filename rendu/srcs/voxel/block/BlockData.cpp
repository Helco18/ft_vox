#include "BlockData.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"
#include <iostream>

BlockData::BlockDataRegistry BlockData::_registry;

void BlockData::init()
{
	_registry.emplace(0, BlockData("air", false));
	_registry.emplace(1, BlockData("stone", true));
	_registry.emplace(2, BlockData("moi", true));
}

BlockData BlockData::getBlockData(uint8_t type)
{
	BlockDataRegistry::iterator it = _registry.find(type);
	if (it != _registry.end())
		return it->second;
	throw VoxelException("Invalid BlockData #" + toString(type) + " requested.");
}
