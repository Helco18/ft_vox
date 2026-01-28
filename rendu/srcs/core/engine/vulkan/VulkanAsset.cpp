#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"
#include <cstdlib>

AssetID VulkanEngine::uploadAsset(Asset & asset, PipelineID pipelineID)
{
	PendingAsset * pendingAsset = reinterpret_cast<PendingAsset *>(calloc(1, sizeof(PendingAsset)));
	pendingAsset->asset = &asset;
	pendingAsset->pipelineID = pipelineID;

	AssetID assetID = _nextAssetID++;
	asset.assetID = assetID;
	if (asset.vertices.data)
	{
		_threadPool.submitTask([this, pendingAsset]() {
			try
			{
				_createVertexBuffer(pendingAsset);
				if (!pendingAsset->asset->indices.empty())
					_createIndexBuffer(pendingAsset);
			} catch (const vk::OutOfDeviceMemoryError & e)
			{
				Logger::log(ENGINE_VULKAN, FATAL, e.what());
			}
		});
	}
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

void VulkanEngine::_uploadPendingAssets()
{
	std::lock_guard<std::mutex> lg(_pendingAssetMutex);
	if (_pendingAssets.empty() || !_isInitalized.load())
		return;

	std::vector<PendingAsset> nextPendingAssets;
	vk::raii::CommandBuffer commandBuffer = _beginSingleTimeCommands();
	for (PendingAsset * pendingAsset : _pendingAssets)
	{
		Asset * asset = pendingAsset->asset;
		PipelineMap::iterator it = _pipelineMap.find(pendingAsset->pipelineID);
		if (it == _pipelineMap.end())
			continue;
		commandBuffer.copyBuffer(pendingAsset->stagingVertexData.buffer, pendingAsset->vertexData.buffer,
			vk::BufferCopy(0, 0, asset->vertices.size));
		if (!asset->indices.empty())
		{
			commandBuffer.copyBuffer(pendingAsset->stagingIndexData.buffer, pendingAsset->indexData.buffer,
				vk::BufferCopy(0, 0, sizeof(uint32_t) * asset->indices.size()));
		}
	}
	_endSingleTimeCommands(commandBuffer);
	for (PendingAsset * pendingAsset : _pendingAssets)
	{
		Asset * asset = pendingAsset->asset;
		AssetData assetData;
		assetData.asset = asset;
		assetData.vbo = std::move(pendingAsset->vertexData);
		assetData.ibo = std::move(pendingAsset->indexData);
		assetData.pipelineID = pendingAsset->pipelineID;
		_assetDataCache.try_emplace(asset->assetID, std::move(assetData));
	}
	std::for_each(_pendingAssets.begin(), _pendingAssets.end(), [](PendingAsset * pendingAsset) { free(pendingAsset); });
	_pendingAssets.clear();
}
