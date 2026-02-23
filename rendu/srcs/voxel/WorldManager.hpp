#pragma once

#include "World.hpp"
#include <optional>

class WorldManager
{
	public:
		WorldManager() = delete;
		~WorldManager() = delete;

		static World *	getWorld(const std::string & name);
		static void		createWorld(const std::string & name, std::optional<uint32_t> optSeed = std::nullopt);
		static void		loadWorld(const std::string & name);
		static void		destroy();

	private:
		typedef std::unordered_map<std::string, World *> WorldMap;

		static WorldMap	_worldMap;
};
