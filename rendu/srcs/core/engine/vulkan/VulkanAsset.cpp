#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"

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
			asset.isUploaded = true;
		} catch (const vk::OutOfDeviceMemoryError & e)
		{
			Logger::log(ENGINE_VULKAN, FATAL, e.what());
		}
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
