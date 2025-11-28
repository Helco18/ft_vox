#pragma once

#include "World.hpp"

class WorldManager
{
	public:
		WorldManager() = delete;
		~WorldManager() = delete;

		static void		createWorld(const std::string & name);
		static void		destroy();
	private:
		typedef std::unordered_map<std::string, World *> WorldMap;

		static WorldMap	_worldMap;
};
