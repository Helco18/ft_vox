#pragma once

#include "AEngine.hpp"
#include "glm/glm.hpp"

#define CROSSHAIR_SCALE 25.0f

struct CrosshairBuffer
{
	glm::mat4	proj;
	glm::mat4	model;
};

class Crosshair
{
	public:
		Crosshair() {};
		~Crosshair() {};

		void						generateMesh();
		void						uploadAsset(AEngine * engine);
		void						drawAsset(AEngine * engine, int width, int height);
		void						unload(AEngine * engine);

	private:
		Asset						_asset;
		std::vector<glm::vec2>		_vertices;
		std::vector<uint32_t>		_indices;
		CrosshairBuffer				_modelproj;
};
