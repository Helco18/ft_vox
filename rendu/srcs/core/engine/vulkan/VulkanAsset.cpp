#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"

AssetID VulkanEngine::uploadAsset(Asset & asset, PipelineID pipelineID)
{
	AssetID assetID = _nextAssetID++;
	asset.assetID = assetID;
	AssetData assetData;
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
	_assetDataCache.emplace_back(std::move(assetData));
	return assetID;
}

void VulkanEngine::drawAsset(AssetID assetID, PipelineID pipelineID)
{
	AssetData & assetData = _assetDataCache[assetID];
	if (assetData.asset == nullptr)
		return;
	Asset * asset = assetData.asset;
	if (!asset->vertices.data)
		return;
	_pipelineAssetMap[pipelineID].push_back(asset);
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
