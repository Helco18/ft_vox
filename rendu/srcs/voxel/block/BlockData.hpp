#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>

enum BlockFace
{
	SOUTH,
	NORTH,
	WEST,
	EAST,
	TOP,
	BOTTOM
};

enum BlockType
{
	AIR,
	DIRT,
	GRASS,
	WATER
};

class BlockData
{
	public:
		BlockData(const std::string & name, bool isVisible, bool isLiquid, const std::vector<std::string> & textures): _name(name), _isVisible(isVisible),
			_isLiquid(isLiquid), _textures(textures) {}
		~BlockData() {}

		static void					init();
		static BlockData &			getBlockData(uint8_t id);

		const std::string &			getName() const { return _name; }
		bool						isVisible() const { return _isVisible; }
		bool						isLiquid() const { return _isLiquid; }
		const std::string &			getTexturePath(int blockFace);

	private:
		typedef std::unordered_map<uint8_t, BlockData>	BlockDataRegistry;

		static BlockDataRegistry	_dataRegistry;

		std::string					_name;
		bool						_isVisible;
		bool						_isLiquid;
		std::vector<std::string>	_textures;
};
