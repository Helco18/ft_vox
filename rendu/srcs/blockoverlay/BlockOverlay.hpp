#pragma once

#include "World.hpp"
#include "glm/glm.hpp"
#include "AEngine.hpp"

class BlockOverlay
{
	public:
		BlockOverlay() {};
		~BlockOverlay() {};

		void						generateMesh();
		void						uploadAsset(AEngine * engine);
		void						drawAsset(AEngine * engine, const TargetedBlock & targetedBlock);
		void						unload(AEngine * engine);

	private:
		Asset						_asset;
		std::vector<glm::vec3>		_vertices;
		std::vector<uint32_t>		_indices;
		glm::mat4					_model;
};
