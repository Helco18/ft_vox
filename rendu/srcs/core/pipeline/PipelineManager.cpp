#include "PipelineManager.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"

PipelineManager::PipelineMap PipelineManager::_pipelineMap;

void PipelineManager::init(AEngine * engine)
{
	PipelineInfo pipelineInfo;
	pipelineInfo.shaderName = "voxel";
	_uploadPipeline(engine, pipelineInfo, PIPELINE_VOXEL);
	pipelineInfo.polygonMode = LINE;
	_uploadPipeline(engine, pipelineInfo, PIPELINE_WIREFRAME);
}

void PipelineManager::_uploadPipeline(AEngine * engine, PipelineInfo & pipelineInfo, PipelineType pipelineType)
{
	PipelineID pipelineID = engine->uploadPipeline(pipelineInfo);
	_pipelineMap.try_emplace(pipelineType, pipelineID);
}

PipelineID PipelineManager::getPipeline(PipelineType pipelineType)
{
	PipelineMap::iterator it = _pipelineMap.find(pipelineType);
	if (it != _pipelineMap.end())
		return it->second;
	throw PipelineException("Couldn't get pipeline type: " + toString(pipelineType) + ".");
}
