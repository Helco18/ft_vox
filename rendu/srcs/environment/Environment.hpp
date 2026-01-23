#pragma once

#include "Player.hpp"
#include "WindowManager.hpp"

#define WORLD_NAME "bozoandzibocircus"

class Environment
{
	public:
		Environment() {};
		~Environment();

		void		init(EngineType engineType);
		void		loop();

		void		stop() { _running = false; }

	private:
		WindowManager *	_windowManager = nullptr;
		EngineType		_engineType;
		Player			_player;
		bool			_running = true;
};
