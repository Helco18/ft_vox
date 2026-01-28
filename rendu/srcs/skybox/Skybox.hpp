#pragma once

#include "AEngine.hpp"
#include "glm/glm.hpp"

struct SkyboxVertex
{
	glm::vec3	position;
	glm::vec3	normal;
};

class Skybox
{
	public:
		Skybox();
		~Skybox();

		void						generateMesh();
		void						uploadAsset(AEngine * engine);
		void						drawAsset(AEngine * engine, PipelineType pipelineType);
		void						unload(AEngine * engine);
	private:
		Asset						_asset;
		std::vector<SkyboxVertex>	_vertices;
		std::vector<uint32_t>		_indices;
};
