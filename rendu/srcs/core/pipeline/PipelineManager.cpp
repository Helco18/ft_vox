#include "PipelineManager.hpp"
#include "CustomExceptions.hpp"
#include "AEngine.hpp"
#include "utils.hpp"
#include <vector>

PipelineManager::PipelineMap PipelineManager::_pipelineMap;

void PipelineManager::init(AEngine * engine)
{
	PipelineInfo pipelineInfoVoxel;
	pipelineInfoVoxel.shaderName = "voxel";
	pipelineInfoVoxel.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	pipelineInfoVoxel.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	pipelineInfoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	pipelineInfoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	pipelineInfoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	pipelineInfoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });

	for (Attribute attribute : pipelineInfoVoxel.attributes)
		pipelineInfoVoxel.attributeSize += attribute.size;
	_uploadPipeline(engine, pipelineInfoVoxel, PIPELINE_VOXEL);

	PipelineInfo pipelineInfoWireframe(pipelineInfoVoxel);
	pipelineInfoWireframe.polygonMode = LINE;
	_uploadPipeline(engine, pipelineInfoWireframe, PIPELINE_WIREFRAME);

	PipelineInfo pipelineInfoBasic;
	pipelineInfoBasic.shaderName = "basic";
	pipelineInfoBasic.attributes.clear();
	pipelineInfoBasic.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	pipelineInfoBasic.attributeSize = sizeof(glm::vec2);
	pipelineInfoBasic.blend = false;
	pipelineInfoBasic.cullMode = OFF;
	pipelineInfoBasic.depthTest = false;
	_uploadPipeline(engine, pipelineInfoBasic, PIPELINE_BASIC);

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
