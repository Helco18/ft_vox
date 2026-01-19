#include "PipelineManager.hpp"
#include "CustomExceptions.hpp"
#include "AEngine.hpp"
#include "utils.hpp"
#include <vector>

PipelineManager::PipelineMap PipelineManager::_pipelineMap;

void PipelineManager::init(AEngine * engine)
{
	PipelineInfo infoVoxel;
	infoVoxel.shaderName = "voxel";
	infoVoxel.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	infoVoxel.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	infoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	infoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	infoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	infoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	
	DescriptorInfo cameraMatrix;
	cameraMatrix.binding = 0;
	cameraMatrix.count = 1;
	cameraMatrix.size = sizeof(UniformBuffer);
	cameraMatrix.stage = ShaderStage::VERTEX;
	cameraMatrix.type = DescriptorType::UNIFORM_BUFFER;

	DescriptorInfo textureAtlas;
	textureAtlas.binding = 1;
	textureAtlas.count = 1;
	textureAtlas.stage = ShaderStage::FRAGMENT;
	textureAtlas.type = DescriptorType::COMBINED_IMAGE_SAMPLER;

	DescriptorInfo chunkValues;
	chunkValues.binding = 2;
	chunkValues.count = 1;
	chunkValues.size = sizeof(float);
	chunkValues.stage = ShaderStage::FRAGMENT;
	chunkValues.type = DescriptorType::UNIFORM_BUFFER;

	infoVoxel.descriptors.push_back(cameraMatrix);
	infoVoxel.descriptors.push_back(textureAtlas);
	infoVoxel.descriptors.push_back(chunkValues);
	for (Attribute attribute : infoVoxel.attributes)
		infoVoxel.attributeSize += attribute.size;
	_uploadPipeline(engine, infoVoxel, PIPELINE_VOXEL);

	// PipelineInfo infoWireframe(infoVoxel);
	// infoWireframe.polygonMode = LINE;
	// _uploadPipeline(engine, infoWireframe, PIPELINE_WIREFRAME);

	// PipelineInfo infoBasic;
	// infoBasic.shaderName = "basic";
	// infoBasic.attributes.clear();
	// infoBasic.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	// infoBasic.attributeSize = sizeof(glm::vec2);
	// infoBasic.blend = false;
	// infoBasic.cullMode = OFF;
	// infoBasic.depthTest = false;
	// _uploadPipeline(engine, infoBasic, PIPELINE_BASIC);
}

void PipelineManager::_uploadPipeline(AEngine * engine, PipelineInfo & pipelineInfo, PipelineType pipelineType)
{
	pipelineInfo.id = engine->uploadPipeline(pipelineInfo);
	_pipelineMap.try_emplace(pipelineType, pipelineInfo);
}

const PipelineInfo & PipelineManager::getPipeline(PipelineType pipelineType)
{
	PipelineMap::iterator it = _pipelineMap.find(pipelineType);
	if (it != _pipelineMap.end())
		return it->second;
	throw PipelineException("Couldn't get pipeline type: " + toString(pipelineType) + ".");
}
