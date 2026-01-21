#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"

AssetID VulkanEngine::uploadAsset(Asset & asset, PipelineID pipelineID)
{
	PendingAsset pendingAsset;
	pendingAsset.asset = &asset;
	pendingAsset.pipelineID = pipelineID;

	AssetID assetID = _nextAssetID++;
	asset.assetID = assetID;
	if (asset.vertices.data)
	{
		try
		{
			_createVertexBuffer(pendingAsset);
			_createIndexBuffer(pendingAsset);
		} catch (const vk::OutOfDeviceMemoryError & e)
		{
			throw VulkanException(e.what());
		}
		_pendingAssets.push_back(std::move(pendingAsset));
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
