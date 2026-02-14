#pragma once

#include "AEngine.hpp"
#include "glm/glm.hpp"

class Crosshair
{
	public:
		Crosshair() {};
		~Crosshair() {};

		void						generateMesh();
		void						uploadAsset(AEngine * engine);
		void						drawAsset(AEngine * engine);
		void						unload(AEngine * engine);

	private:
		Asset						_asset;
		std::vector<glm::vec2>		_vertices;
		std::vector<uint32_t>		_indices;
};
