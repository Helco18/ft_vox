#include "PipelineManager.hpp"
#include "Chunk.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"
#include "Camera.hpp"
#include "TextureAtlas.hpp"

PipelineManager::PipelineMap PipelineManager::_pipelineMap;

static void uploadLines(AEngine * engine)
{
	PipelineInfo infoLines;
	infoLines.shaderName = "lines";
	infoLines.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	infoLines.attributeSize = sizeof(glm::vec3);
	infoLines.drawMode = DrawMode::LINES;
	infoLines.polygonMode = PolygonMode::LINE;
	infoLines.cullMode = CullMode::OFF;

	DescriptorInfo cameraMatrix;
	cameraMatrix.name = "CameraMatrix";
	cameraMatrix.binding = 0;
	cameraMatrix.count = 1;
	cameraMatrix.size = sizeof(CameraBuffer);
	cameraMatrix.stage = ShaderStage::VERTEX;
	cameraMatrix.type = DescriptorType::UNIFORM_BUFFER;

	infoLines.descriptors.push_back(cameraMatrix);
	PipelineManager::uploadPipeline(engine, infoLines, PIPELINE_LINES);
}

static void uploadVoxel(AEngine * engine)
{
	PipelineInfo infoVoxel;
	infoVoxel.shaderName = "voxel";
	infoVoxel.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	infoVoxel.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	infoVoxel.attributes.push_back({ sizeof(float), 1, FLOAT, false });
	infoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	infoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	infoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });
	infoVoxel.attributes.push_back({ sizeof(glm::vec2), 2, FLOAT2, false });

	DescriptorInfo cameraMatrix;
	cameraMatrix.name = "CameraMatrix";
	cameraMatrix.binding = 0;
	cameraMatrix.count = 1;
	cameraMatrix.size = sizeof(CameraBuffer);
	cameraMatrix.stage = ShaderStage::VERTEX;
	cameraMatrix.type = DescriptorType::UNIFORM_BUFFER;

	DescriptorInfo textureAtlas;
	textureAtlas.name = "texture_0";
	textureAtlas.binding = 1;
	textureAtlas.count = 1;
	textureAtlas.stage = ShaderStage::FRAGMENT;
	textureAtlas.type = DescriptorType::COMBINED_IMAGE_SAMPLER;
	TextureInfo textureInfo;
	textureInfo.width = TextureAtlas::getWidth();
	textureInfo.height = TextureAtlas::getHeight();
	textureInfo.data = TextureAtlas::getData();
	textureInfo.colorChannels = TextureAtlas::getColorChannels();
	textureAtlas.textureInfo = textureInfo;

	DescriptorInfo chunkData;
	chunkData.name = "ChunkData";
	chunkData.binding = 2;
	chunkData.count = 1;
	chunkData.size = sizeof(ChunkData);
	chunkData.stage = ShaderStage::VERTEX;
	chunkData.type = DescriptorType::PUSH_CONSTANT;

	infoVoxel.uniformSize = sizeof(ChunkData);
	infoVoxel.descriptors.push_back(cameraMatrix);
	infoVoxel.descriptors.push_back(textureAtlas);
	infoVoxel.descriptors.push_back(chunkData);
	for (Attribute attribute : infoVoxel.attributes)
		infoVoxel.attributeSize += attribute.size;
	PipelineManager::uploadPipeline(engine, infoVoxel, PIPELINE_VOXEL);

	PipelineInfo infoWireframe(infoVoxel);
	infoWireframe.polygonMode = LINE;
	PipelineManager::uploadPipeline(engine, infoWireframe, PIPELINE_WIREFRAME);
}

static void uploadSkybox(AEngine * engine)
{
	PipelineInfo infoSkybox;
	infoSkybox.shaderName = "skybox";
	infoSkybox.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	infoSkybox.attributes.push_back({ sizeof(glm::vec3), 3, FLOAT3, false });
	infoSkybox.cullMode = CullMode::OFF;
	infoSkybox.blend = false;
	infoSkybox.depthTest = false;

	DescriptorInfo cameraMatrix;
	cameraMatrix.name = "CameraMatrix";
	cameraMatrix.binding = 0;
	cameraMatrix.count = 1;
	cameraMatrix.size = sizeof(CameraBuffer);
	cameraMatrix.stage = ShaderStage::VERTEX;
	cameraMatrix.type = DescriptorType::UNIFORM_BUFFER;

	infoSkybox.descriptors.push_back(cameraMatrix);
	for (Attribute attribute : infoSkybox.attributes)
		infoSkybox.attributeSize += attribute.size;
	PipelineManager::uploadPipeline(engine, infoSkybox, PIPELINE_SKYBOX);
}

void PipelineManager::init(AEngine * engine)
{
	uploadVoxel(engine);
	uploadLines(engine);
	uploadSkybox(engine);

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

void PipelineManager::uploadPipeline(AEngine * engine, PipelineInfo & pipelineInfo, PipelineType pipelineType)
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
