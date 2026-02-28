#pragma once

#include "Player.hpp"
#include "WindowManager.hpp"

#define WORLD_NAME "bozoandzibocircus"

struct ProgramParams
{
	uint8_t		renderDistance = 8;
	EngineType	engineType = VULKAN;
	uint8_t		fov = 80;
	bool		vsync = false;
	uint32_t	seed = 42;
	glm::vec3	spawnLocation = glm::vec3(0.0f);
};

class Environment
{
	public:
		Environment() {};
		~Environment();

		void		init(ProgramParams & programParams);
		void		loop();
		
		Player &	getPlayer() { return _player; }

		void		stop() { _running = false; }

	private:
		WindowManager *	_windowManager = nullptr;
		EngineType		_engineType;
		Player			_player;
		bool			_running = true;
};
