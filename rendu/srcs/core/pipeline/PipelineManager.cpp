#include "PipelineManager.hpp"
#include "CustomExceptions.hpp"
#include "AEngine.hpp"
#include "utils.hpp"
#include <vector>

PipelineManager::PipelineMap PipelineManager::_pipelineMap;

void PipelineManager::init(AEngine * engine)
{
	PipelineInfo pipelineInfo;
	pipelineInfo.shaderName = "voxel";
	pipelineInfo.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	pipelineInfo.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	pipelineInfo.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	pipelineInfo.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	pipelineInfo.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	pipelineInfo.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });

	for (Attribute attribute : pipelineInfo.attributes)
		pipelineInfo.attributeSize += attribute.size;
	_uploadPipeline(engine, pipelineInfo, PIPELINE_VOXEL);

	pipelineInfo.polygonMode = LINE;
	_uploadPipeline(engine, pipelineInfo, PIPELINE_WIREFRAME);
}

void PipelineManager::_uploadPipeline(AEngine * engine, PipelineInfo & pipelineInfo, PipelineType pipelineType)
{
	pipelineInfo.id = engine->uploadPipeline(pipelineInfo);
	_pipelineMap.try_emplace(pipelineType, pipelineInfo);
}

Pipeline PipelineManager::getPipeline(PipelineType pipelineType)
{
	PipelineMap::iterator it = _pipelineMap.find(pipelineType);
	if (it != _pipelineMap.end())
		return it->second;
	throw PipelineException("Couldn't get pipeline type: " + toString(pipelineType) + ".");
}
