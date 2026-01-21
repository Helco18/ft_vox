#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"

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
	_assetMap.try_emplace(asset.assetID, &asset);
	return assetID;
}

void VulkanEngine::drawAsset(AssetID assetID, PipelineID pipelineID)
{
	AssetMap::iterator assetit = _assetMap.find(assetID);
	if (assetit != _assetMap.end())
	{
		Asset * asset = assetit->second;
		if (!asset->vertices.data)
			return;
		_pipelineAssetMap[pipelineID].push_back(asset);
	}
}

void VulkanEngine::unloadAsset(AssetID assetID)
{
	AssetMap::iterator assetit = _assetMap.find(assetID);
	if (assetit != _assetMap.end())
		_assetMap.erase(assetit);
	AssetDataCache::iterator datait = _assetDataCache.find(assetID);
	if (datait == _assetDataCache.end())
		return;
	_assetDataCache.erase(datait);
}
