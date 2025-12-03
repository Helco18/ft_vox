#include "WorldManager.hpp"
#include "utils.hpp"
#include "Logger.hpp"
#include <iostream>

WorldManager::WorldMap WorldManager::_worldMap;

void WorldManager::createWorld(const std::string & name)
{
	_worldMap[name] = new World(name);
	_worldMap[name]->load();

	Logger::log(VOXEL, INFO, "Created world " + name + ".");
}

World * WorldManager::getWorld(const std::string & name)
{
	WorldMap::iterator it = _worldMap.find(name);
	if (it != _worldMap.end())
		return it->second;
	return nullptr;
}

void WorldManager::destroy()
{
	for (std::pair<std::string, World *> entry : _worldMap)
	{
		World * world = entry.second;
		delete world;
	}
}
