#include "Profiler.hpp"
#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"

AssetID VulkanEngine::uploadAsset(Asset & asset, PipelineID pipelineID)
{
	Profiler p("VulkanEngine::uploadAsset");
	AssetID assetID = _nextAssetID++;
	asset.assetID = assetID;
	PendingAsset pendingAsset;
	pendingAsset.pipelineID = pipelineID;
	pendingAsset.asset = &asset;
	if (asset.vertices.data)
	{
		try
		{
			_createVertexBuffer(pendingAsset);
			if (!asset.indices.empty())
				_createIndexBuffer(pendingAsset);
		} catch (const vk::OutOfDeviceMemoryError & e)
		{
			Logger::log(ENGINE_VULKAN, FATAL, e.what());
		}
	}
	_pendingAssets.emplace_back(std::move(pendingAsset));
	return assetID;
}

void VulkanEngine::drawAsset(AssetID assetID, PipelineID pipelineID)
{
	if (assetID >= _assetDataCache.size())
		return;
	AssetData & assetData = _assetDataCache[assetID];
	if (assetData.asset == nullptr)
		return;
	Asset * asset = assetData.asset;
	if (!asset->vertices.data || !asset->isUploaded)
		return;
	_drawableAssets[pipelineID].push_back(asset);
}

void VulkanEngine::unloadAsset(AssetID assetID)
{
	if (assetID >= _assetDataCache.size())
		return;
	AssetData & assetData = _assetDataCache[assetID];
	if (assetData.asset == nullptr)
		return;
	else if (!assetData.asset->isUploaded)
		return;
	_assetDataCache.erase(_assetDataCache.begin() + assetID);
}

void VulkanEngine::_processPendingAssets()
{
	if (_pendingAssets.empty())
		return;
	vk::raii::CommandBuffer commandBuffer = _beginSingleTimeCommands();
	for (PendingAsset & pendingAsset : _pendingAssets)
	{
		Asset * asset = pendingAsset.asset;
		commandBuffer.copyBuffer(pendingAsset.stagingVertexData.buffer, pendingAsset.vertexData.buffer,
			vk::BufferCopy(0, 0, asset->vertices.size));
		if (!asset->indices.empty())
		{
			commandBuffer.copyBuffer(pendingAsset.stagingIndexData.buffer, pendingAsset.indexData.buffer,
				vk::BufferCopy(0, 0, sizeof(uint32_t) * asset->indices.size()));
		}
	}
	_endSingleTimeCommands(commandBuffer);
	for (PendingAsset & pendingAsset : _pendingAssets)
	{
		Asset * asset = pendingAsset.asset;
		AssetData assetData;
		assetData.asset = asset;
		assetData.vbo = std::move(pendingAsset.vertexData);
		assetData.ibo = std::move(pendingAsset.indexData);
		asset->isUploaded = true;
		_assetDataCache.emplace_back(std::move(assetData));
	}
	_pendingAssets.clear();
}
