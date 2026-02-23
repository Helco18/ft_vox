#include "WorldManager.hpp"
#include "Logger.hpp"

WorldManager::WorldMap WorldManager::_worldMap;

World * WorldManager::getWorld(const std::string & name)
{
	WorldMap::iterator it = _worldMap.find(name);
	if (it != _worldMap.end())
		return it->second;
	return nullptr;
}

void WorldManager::createWorld(const std::string & name, std::optional<uint32_t> optSeed)
{
	uint32_t seed;
	if (!optSeed.has_value())
		seed = time(NULL);
	else
		seed = optSeed.value();
	_worldMap[name] = new World(name, seed);

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
