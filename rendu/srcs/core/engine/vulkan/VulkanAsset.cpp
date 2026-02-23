#include "Profiler.hpp"
#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"
#include "utils.hpp"

AssetID VulkanEngine::uploadAsset(Asset & asset, PipelineID pipelineID)
{
	Profiler p("VulkanEngine::uploadAsset");
	AssetID assetID = _nextAssetID++;
	asset.assetID = assetID;
	PendingAsset pendingAsset;
	pendingAsset.pipelineID = pipelineID;
	pendingAsset.asset = &asset;
	_pendingAssets.emplace_back(std::move(pendingAsset));
	return assetID;
}

void VulkanEngine::drawAsset(AssetID assetID, PipelineID pipelineID)
{
	if (_assetDataCache.empty())
		return;
	AssetData & assetData = _assetDataCache[assetID];
	Asset * asset = assetData.asset;
	if (!asset || !asset->isUploaded || !asset->vertices.data)
		return;
	_drawableAssets[pipelineID].push_back(asset);
}

void VulkanEngine::unloadAsset(AssetID assetID)
{
	if (_assetDataCache.empty())
		return;
	AssetData & assetData = _assetDataCache[assetID];
	Asset * asset = assetData.asset;
	if (!asset || !asset->isUploaded)
		return;
	asset->isUploaded = false;
	_pendingUnloads.push_back(assetID);
}

void VulkanEngine::_processPendingAssets()
{
	if (_pendingAssets.empty())
		return;
	_transferCommandBuffers[_currentFrame].reset();
	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	_transferCommandBuffers[_currentFrame].begin(commandBufferBeginInfo);
	for (PendingAsset & pendingAsset : _pendingAssets)
	{
		Asset * asset = pendingAsset.asset;
		if (asset->vertices.data)
		{
			try
			{
				_createVertexBuffer(pendingAsset);
				if (!asset->indices.empty())
					_createIndexBuffer(pendingAsset);
			} catch (const vk::OutOfDeviceMemoryError & e)
			{
				Logger::log(ENGINE_VULKAN, FATAL, e.what());
			}
		}
		_transferCommandBuffers[_currentFrame].copyBuffer(pendingAsset.stagingVertexData.buffer, pendingAsset.vertexData.buffer,
			vk::BufferCopy(0, 0, asset->vertices.size));
		if (!asset->indices.empty())
		{
			_transferCommandBuffers[_currentFrame].copyBuffer(pendingAsset.stagingIndexData.buffer, pendingAsset.indexData.buffer,
				vk::BufferCopy(0, 0, sizeof(uint32_t) * asset->indices.size()));
		}
	}
	_transferCommandBuffers[_currentFrame].end();
	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &*_transferCommandBuffers[_currentFrame];
	_transferQueue.submit(submitInfo);
	_transferQueue.waitIdle();
	for (PendingAsset & pendingAsset : _pendingAssets)
	{
		Asset * asset = pendingAsset.asset;
		AssetData assetData;
		assetData.pipelineID = pendingAsset.pipelineID;
		assetData.asset = asset;
		assetData.vbo = std::move(pendingAsset.vertexData);
		assetData.ibo = std::move(pendingAsset.indexData);
		asset->isUploaded = true;
		_assetDataCache.try_emplace(asset->assetID, std::move(assetData));
	}
	_pendingAssets.clear();
}

void VulkanEngine::_processPendingUnloads()
{
	if (_pendingUnloads.empty())
		return;
	for (AssetID assetID : _pendingUnloads)
		_assetDataCache.erase(assetID);
	_pendingUnloads.clear();
}
