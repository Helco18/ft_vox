#include "Crosshair.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

void Crosshair::generateMesh()
{
	_vertices = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f)
	};

	_indices = {
		0, 1, 2,
		2, 3, 0
	};

	_asset.vertices.vertexCount = _vertices.size();
	_asset.vertices.size = _vertices.size() * sizeof(glm::vec2);
	_asset.vertices.stride = sizeof(glm::vec2);
	_asset.vertices.data = std::move(_vertices.data());
	_asset.indices = _indices;
}

void Crosshair::uploadAsset(AEngine * engine)
{
	engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_CROSSHAIR).id);
}

void Crosshair::drawAsset(AEngine * engine)
{
	engine->drawAsset(_asset.assetID, PipelineManager::getPipeline(PIPELINE_CROSSHAIR).id);
}

void Crosshair::unload(AEngine * engine)
{
	engine->unloadAsset(_asset.assetID);
}
