#include "WorldManager.hpp"
#include "utils.hpp"

WorldManager::~WorldManager()
{
	for (std::pair<std::string, World *> entry : _worldMap)
	{
		World * world = entry.second;
		SAFE_DELETE(world);
	}
}

void WorldManager::createWorld(const std::string & name)
{
	_worldMap[name] = new World(name);
	_worldMap[name]->load();
}
