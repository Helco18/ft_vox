#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"
#include <cstdlib>

AssetID VulkanEngine::uploadAsset(Asset & asset, PipelineID pipelineID)
{
	AssetID assetID = _nextAssetID++;
	asset.assetID = assetID;
	AssetData & assetData = _assetDataCache[assetID];
	assetData.pipelineID = pipelineID;
	assetData.asset = &asset;
	if (asset.vertices.data)
	{
		try
		{
			_createVertexBuffer(assetData);
			if (!asset.indices.empty())
				_createIndexBuffer(assetData);
		} catch (const vk::OutOfDeviceMemoryError & e)
		{
			Logger::log(ENGINE_VULKAN, FATAL, e.what());
		}
	}
	asset.isUploaded = true;
	return assetID;
}

void VulkanEngine::drawAsset(AssetID assetID, PipelineID pipelineID)
{
	AssetDataCache::iterator assetit = _assetDataCache.find(assetID);
	if (assetit != _assetDataCache.end())
	{
		AssetData & assetData = assetit->second;
		Asset * asset = assetData.asset;
		if (!asset->vertices.data)
			return;
		_pipelineAssetMap[pipelineID].push_back(asset);
	}
}

void VulkanEngine::unloadAsset(AssetID assetID)
{
	AssetDataCache::iterator datait = _assetDataCache.find(assetID);
	if (datait == _assetDataCache.end())
		return;
	_assetDataCache.erase(datait);
}

// void VulkanEngine::_uploadPendingAssets()
// {
// 	if (_pendingAssets.empty() || !_isInitalized.load())
// 		return;

// 	std::vector<PendingAsset> nextPendingAssets;
// 	vk::raii::CommandBuffer commandBuffer = _beginSingleTimeCommands();
// 	std::lock_guard<std::mutex> lg(_pendingAssetMutex);
// 	for (std::pair<const AssetID, PendingAsset> & assetPair : _pendingAssets)
// 	{
// 		PendingAsset & pendingAsset = assetPair.second;
// 		Asset * asset = pendingAsset.asset;
// 		PipelineMap::iterator it = _pipelineMap.find(pendingAsset.pipelineID);
// 		if (it == _pipelineMap.end())
// 			continue;
// 		commandBuffer.copyBuffer(pendingAsset.stagingVertexData.buffer, pendingAsset.vertexData.buffer,
// 			vk::BufferCopy(0, 0, asset->vertices.size));
// 		if (!asset->indices.empty())
// 		{
// 			commandBuffer.copyBuffer(pendingAsset.stagingIndexData.buffer, pendingAsset.indexData.buffer,
// 				vk::BufferCopy(0, 0, sizeof(uint32_t) * asset->indices.size()));
// 		}
// 		pendingAsset.isReady = true;
// 	}
// 	for (std::pair<const AssetID, PendingAsset> & assetPair : _pendingAssets)
// 	{
// 		PendingAsset & pendingAsset = assetPair.second;
// 		if (!pendingAsset.isReady)
// 			continue;
// 		Asset * asset = pendingAsset.asset;
// 		AssetData assetData;
// 		assetData.asset = asset;
// 		assetData.vbo = std::move(pendingAsset.vertexData);
// 		assetData.ibo = std::move(pendingAsset.indexData);
// 		assetData.pipelineID = pendingAsset.pipelineID;
// 		_assetDataCache.try_emplace(asset->assetID, std::move(assetData));
// 	}
// 	_endSingleTimeCommands(commandBuffer);
// 	std::for_each(_pendingAssets.begin(), _pendingAssets.end(), [](std::pair<const AssetID, PendingAsset> & assetPair)
// 		{ assetPair.second.asset->isUploaded = true; } );
// 	_pendingAssets.clear();
// }
