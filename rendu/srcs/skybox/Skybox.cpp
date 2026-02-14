#include "Skybox.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

void Skybox::generateMesh()
{
	for (float latitude = 0.0f; latitude <= 16.0f; ++latitude)
	{
		float v = latitude / 16.0f;
		float theta = v * glm::pi<float>();
		for (float longitude = 0.0f; longitude <= 16.0f; ++longitude)
		{
			float u = longitude / 16.0f;
			float phi = u * glm::two_pi<float>();
			
			glm::vec3 pos;
			pos.x = cos(phi) * sin(theta);
			pos.y = cos(theta);
			pos.z = sin(phi) * sin(theta);

			SkyboxVertex vert;
			vert.position = pos * 5.0f;
			vert.normal = -glm::normalize(pos);

			_vertices.push_back(vert);
		}
	}

	for (float latitude = 0.0f; latitude <= 16.0f; ++latitude)
	{
		for (float longitude = 0.0f; longitude <= 16.0f; ++longitude)
		{
			float a = latitude * 17.0f + longitude;
			float b = a + 17.0f;
			float c = a + 1.0f;
			float d = b + 1.0f;

			_indices.push_back(a);
			_indices.push_back(b);
			_indices.push_back(c);

			_indices.push_back(c);
			_indices.push_back(b);
			_indices.push_back(d);
		}
	}

	_asset.vertices.data = _vertices.data();
	_asset.vertices.vertexCount = _vertices.size();
	_asset.indices = _indices;
	_asset.vertices.size = _vertices.size() * sizeof(SkyboxVertex);
}

void Skybox::uploadAsset(AEngine * engine)
{
	engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_SKYBOX).id);
}

void Skybox::drawAsset(AEngine * engine)
{
	engine->drawAsset(_asset.assetID, PipelineManager::getPipeline(PIPELINE_SKYBOX).id);
}

void Skybox::unload(AEngine * engine)
{
	engine->unloadAsset(_asset.assetID);
}
