#include "Crosshair.hpp"
#include "Logger.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

void Crosshair::generateMesh()
{
	_vertices = {
		glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f),
	};

	_indices = {
		0, 1, 2,
		2, 3, 0
	};

	_asset.vertices.vertexCount = _vertices.size();
	_asset.vertices.size = _vertices.size() * sizeof(glm::vec2);
	_asset.vertices.stride = sizeof(glm::vec2);
	_asset.vertices.data = _vertices.data();
	_asset.indices = _indices;
}

void Crosshair::uploadAsset(AEngine * engine)
{
	engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_CROSSHAIR).id);
}

void Crosshair::drawAsset(AEngine * engine, int width, int height)
{
	static int oldWidth = 0;
	static int oldHeight = 0;
	static PipelineID pipelineID = PipelineManager::getPipeline(PIPELINE_CROSSHAIR).id;
	static EngineType lastEngineType = engine->getEngineType();

	if (oldWidth != width || oldHeight != height || lastEngineType != engine->getEngineType())
	{
		_modelproj.proj = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
		_modelproj.model = glm::translate(glm::mat4(1.0f), glm::vec3((static_cast<float>(width) / 2) - CROSSHAIR_SCALE / 2,
			static_cast<float>(height) / 2 - CROSSHAIR_SCALE / 2, 0.0f));
		_modelproj.model = glm::scale(_modelproj.model, glm::vec3(CROSSHAIR_SCALE, CROSSHAIR_SCALE, 1.0f));
		oldWidth = width;
		oldHeight = height;
		lastEngineType = engine->getEngineType();
	}
	engine->updateUniformBuffer(pipelineID, 1, &_modelproj, sizeof(CrosshairBuffer));
	engine->drawAsset(_asset.assetID, pipelineID);
}

void Crosshair::unload(AEngine * engine)
{
	engine->unloadAsset(_asset.assetID);
}
