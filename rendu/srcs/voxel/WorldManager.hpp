#pragma once

#include "World.hpp"

class WorldManager
{
	public:
		WorldManager();
		~WorldManager();

		void		createWorld(const std::string & name);
	private:
		typedef std::unordered_map<std::string, World *> WorldMap;

		WorldMap	_worldMap;
};
