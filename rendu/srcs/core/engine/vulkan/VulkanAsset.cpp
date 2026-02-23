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
	AssetData assetData;
	asset.isUploaded = true;
	assetData.asset = &asset;
	_pendingAssets.emplace_back(std::move(pendingAsset));
	_assetDataCache[assetID] = std::move(assetData);
	return assetID;
}

void VulkanEngine::drawAsset(AssetID assetID, PipelineID pipelineID)
{
	if (_assetDataCache.empty())
		return;
	AssetData & assetData = _assetDataCache[assetID];
	Asset * asset = assetData.asset;
	if (!asset || !asset->isUploaded || !asset->vertices.data)
	{
		Logger::log(ENGINE_VULKAN, DEBUG, "POUET POUET :)))))))))))))))");
		return;
	}
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
	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	_transferCommandBuffer.begin(commandBufferBeginInfo);
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
		_transferCommandBuffer.copyBuffer(pendingAsset.stagingVertexData.buffer, pendingAsset.vertexData.buffer,
			vk::BufferCopy(0, 0, asset->vertices.size));
		if (!asset->indices.empty())
		{
			_transferCommandBuffer.copyBuffer(pendingAsset.stagingIndexData.buffer, pendingAsset.indexData.buffer,
				vk::BufferCopy(0, 0, sizeof(uint32_t) * asset->indices.size()));
		}
	}
	_transferCommandBuffer.end();
	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &*_transferCommandBuffer;
	_transferQueue.submit(submitInfo);
	_transferQueue.waitIdle();
	for (PendingAsset & pendingAsset : _pendingAssets)
	{
		Asset * asset = pendingAsset.asset;
		AssetData & assetData = _assetDataCache[asset->assetID];
		assetData.pipelineID = pendingAsset.pipelineID;
		assetData.asset = asset;
		assetData.vbo = std::move(pendingAsset.vertexData);
		assetData.ibo = std::move(pendingAsset.indexData);
	}
	_pendingAssets.clear();
}

void VulkanEngine::_processPendingUnloads()
{
	if (_pendingUnloads.empty())
		return;
	for (AssetID assetID : _nextPendingUnloads)
		_assetDataCache.erase(assetID);
	_nextPendingUnloads.clear();
	_nextPendingUnloads = _pendingUnloads;
}
