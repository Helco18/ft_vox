#include "WorldManager.hpp"
#include "utils.hpp"
#include "Logger.hpp"
#include <iostream>

WorldManager::WorldMap WorldManager::_worldMap;

World * WorldManager::getWorld(const std::string & name)
{
	WorldMap::iterator it = _worldMap.find(name);
	if (it != _worldMap.end())
		return it->second;
	return nullptr;
}

void WorldManager::createWorld(const std::string & name)
{
	_worldMap[name] = new World(name);

	Logger::log(VOXEL, INFO, "Created world " + name + ".");
}

void WorldManager::loadWorld(const std::string & name)
{
	World * world = getWorld(name);
	if (!world)
	{
		Logger::log(VOXEL, WARNING, "Tried loading world " + name + " that doesn't exist.");
		return;
	}
	world->load();
}

void WorldManager::destroy()
{
	for (std::pair<std::string, World *> entry : _worldMap)
	{
		World * world = entry.second;
		delete world;
	}
}
