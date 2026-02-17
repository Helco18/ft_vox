#pragma once

#include "Player.hpp"
#include "WindowManager.hpp"

#define WORLD_NAME "bozoandzibocircus"

struct ProgramParams
{
	uint8_t		renderDistance = 8;
	EngineType	engineType = VULKAN;
	bool		ignoreY = false;
	uint8_t		fov = 80;
	bool		vsync = false;
};

class Environment
{
	public:
		Environment() {};
		~Environment();

		void		init(ProgramParams & programParams);
		void		loop();

		void		stop() { _running = false; }

	private:
		WindowManager *	_windowManager = nullptr;
		EngineType		_engineType;
		Player			_player;
		bool			_running = true;
};
