#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

enum BlockFace
{
	SOUTH,
	NORTH,
	WEST,
	EAST,
	TOP,
	BOTTOM
};

class BlockData
{
	public:
		BlockData(const std::string & name, bool isVisible): _name(name), _isVisible(isVisible) {}
		~BlockData() {}

		static void					init();
		static BlockData			getBlockData(uint8_t id);

		const std::string &			getName() const { return _name; }
		bool						isVisible() const { return _isVisible; }

	private:
		typedef std::unordered_map<uint8_t, BlockData>	BlockDataRegistry;

		static BlockDataRegistry	_registry;

		std::string					_name;
		bool						_isVisible;
};
