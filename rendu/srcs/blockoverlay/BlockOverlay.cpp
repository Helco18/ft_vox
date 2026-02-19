#include "BlockOverlay.hpp"
#include "Logger.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

void BlockOverlay::generateMesh()
{
	_vertices = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 1.0f)
	};

	_indices = {
		4, 5, 6,
		4, 6, 7,

		1, 0, 3,
		1, 3, 2,

		0, 4, 7,
		0, 7, 3,

		5, 1, 2,
		5, 2, 6,

		3, 7, 6,
		3, 6, 2,

		0, 1, 5,
		0, 5, 4
	};

	_asset.vertices.vertexCount = _vertices.size();
	_asset.vertices.size = _vertices.size() * sizeof(glm::vec3);
	_asset.vertices.stride = sizeof(glm::vec3);
	_asset.vertices.data = _vertices.data();
	_asset.indices = _indices;
}

void BlockOverlay::uploadAsset(AEngine * engine)
{	
	engine->uploadAsset(_asset, PipelineManager::getPipeline(PIPELINE_BLOCKOVERLAY).id);
}

void BlockOverlay::drawAsset(AEngine * engine, const TargetedBlock & targetedBlock)
{
	static TargetedBlock lastTargetedBlock = { BlockFace::BOTTOM, glm::vec3(0.0f), BlockType::AIR };
	static PipelineID pipelineID = PipelineManager::getPipeline(PIPELINE_BLOCKOVERLAY).id;
	static EngineType lastEngineType = engine->getEngineType();

	if (!BlockData::getBlockData(targetedBlock.type).isSolid())
		return;
	if (lastTargetedBlock.face != targetedBlock.face || lastTargetedBlock.type != targetedBlock.type || lastTargetedBlock.pos != targetedBlock.pos
		|| lastEngineType != engine->getEngineType())
	{
		lastTargetedBlock = targetedBlock;
		_model = glm::translate(glm::mat4(1.0f), targetedBlock.pos + glm::vec3(0.5f));
		_model = glm::scale(_model, glm::vec3(1.001f));
		_model = glm::translate(_model, glm::vec3(-0.5f)); // Merci mbatty
		lastEngineType = engine->getEngineType();
	}
	engine->updateUniformBuffer(pipelineID, 1, &_model, sizeof(glm::mat4));
	engine->drawAsset(_asset.assetID, pipelineID);
}

void BlockOverlay::unload(AEngine * engine)
{
	engine->unloadAsset(_asset.assetID);
}
