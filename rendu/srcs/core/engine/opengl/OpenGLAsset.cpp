#include "OpenGLEngine.hpp"
#include "CustomExceptions.hpp"
#include "Profiler.hpp"
#include "utils.hpp"

AssetID OpenGLEngine::uploadAsset(Asset & asset, PipelineID pipelineID)
{
	Profiler p("OpenGLAsset::uploadAsset");
	PipelineMap::iterator it = _pipelineMap.find(pipelineID);
	if (it == _pipelineMap.end())
	{
		throw OpenGLException("Failed to upload asset because Pipeline ID: " +
			toString(pipelineID) + " doesn't exist.");
	}
	PipelineLayout & pipelineLayout = it->second;
	PipelineInfo & pipelineInfo = pipelineLayout.pipelineInfo;
	std::vector<Attribute> attributes = pipelineInfo.attributes;
	size_t offset = 0;

	glGenVertexArrays(1, &asset.assetID);
	glBindVertexArray(asset.assetID);

	glGenBuffers(1, &asset.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, asset.vbo);
	glBufferData(GL_ARRAY_BUFFER, asset.vertices.size, asset.vertices.data, GL_STATIC_DRAW);

	for (size_t i = 0; i < attributes.size(); ++i)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, attributes[i].count, GLValueConverter::getType(attributes[i].type), attributes[i].normalized,
		pipelineInfo.attributeSize, (void *)offset);
		offset += attributes[i].size;
	}

	AssetInfo assetInfo;
	assetInfo.asset = &asset;
	for (DescriptorInfo & descriptorInfo : pipelineLayout.pipelineInfo.descriptors)
	{
		if (descriptorInfo.type == DescriptorType::PUSH_CONSTANT)
		{
			UniformBufferStream uniformBuffer;
			uniformBuffer.binding = descriptorInfo.binding;
			uniformBuffer.data = asset.uniforms;
			uniformBuffer.size = descriptorInfo.size;
			glGenBuffers(1, &uniformBuffer.ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer.ubo);
			glBufferData(GL_UNIFORM_BUFFER, uniformBuffer.size, uniformBuffer.data, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, uniformBuffer.binding, uniformBuffer.ubo);
			assetInfo.uniformBufferStreams.push_back(uniformBuffer);
		}
	}

	glGenBuffers(1, &asset.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asset.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, asset.indices.size() * sizeof(uint32_t), asset.indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindVertexArray(0);

	asset.isUploaded = true;
	_assetCache.try_emplace(asset.assetID, assetInfo);

	return asset.assetID;
}

void OpenGLEngine::unloadAsset(AssetID assetID)
{
	AssetCache::iterator it = _assetCache.find(assetID);
	if (it != _assetCache.end())
	{
		AssetInfo & assetInfo = it->second;
		Asset * asset = assetInfo.asset;
		asset->isUploaded = false;
		if (asset->vbo)
			glDeleteBuffers(1, &asset->vbo);
		if (asset->ibo)
			glDeleteBuffers(1, &asset->ibo);
		for (UniformBufferStream & ubs : assetInfo.uniformBufferStreams)
			glDeleteBuffers(1, &ubs.ubo);
		if (asset->assetID)
			glDeleteVertexArrays(1, &asset->assetID);
	}
}

void OpenGLEngine::drawAsset(AssetID assetID, PipelineID pipelineID)
{
	if (_isFramebufferResized)
		_handleResize();

	AssetCache::iterator assetit = _assetCache.find(assetID);
	if (assetit != _assetCache.end())
	{
		AssetInfo & assetInfo = assetit->second;
		Asset * asset = assetInfo.asset;
		if (!asset->vertices.data)
			return;
		_pipelineAssetMap[pipelineID].push_back(asset);
	}
}
