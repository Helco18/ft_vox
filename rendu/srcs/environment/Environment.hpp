#pragma once

#include "WindowManager.hpp"
#include "utils.hpp"

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
		EngineType		_engineType; // A JARTER D'UTILS BORDEL
		bool			_running = true;
};
