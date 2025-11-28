#pragma once

#include "WindowManager.hpp"
#include "utils.hpp"

class Environment
{
	public:
		Environment() {};
		~Environment();

		void		init(EngineType engineType);
		void		loop();

		void		stop() { _running = false; }

	private:
		WindowManager *	_windowManager;
		EngineType		_engineType; // A JARTER D'UTILS BORDEL
		AEngine *		_engine;
		bool			_running = true;
};
