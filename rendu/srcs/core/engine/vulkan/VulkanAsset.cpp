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
	AssetData assetData {};
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
	_pendingUnloads[_currentFrame].push_back(assetID);
}

void VulkanEngine::_processPendingAssets()
{
	if (_pendingAssets.empty())
		return;

	vk::DeviceSize stagingBufferSize = 0;
	for (PendingAsset & pendingAsset : _pendingAssets)
	{
		stagingBufferSize += alignTo(pendingAsset.asset->vertices.size, 16);
		stagingBufferSize += alignTo(pendingAsset.asset->indices.size() * sizeof(uint32_t), 16);
	}
	if (stagingBufferSize > _stagingBufferSize)
	{
		Logger::log(ENGINE_VULKAN, DEBUG, "Reallocating ring buffer with size: " + toString(_stagingBufferSize));
		_createBuffer(stagingBufferSize, vk::BufferUsageFlagBits::eTransferSrc,
						vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
						_stagingBuffer.buffer, _stagingBuffer.memory);
		_stagingBufferSize = stagingBufferSize;
	}
	void * dataStaging = _stagingBuffer.memory.mapMemory(0, stagingBufferSize);
	
	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	_transferCommandBuffer.begin(commandBufferBeginInfo);
	for (PendingAsset & pendingAsset : _pendingAssets)
	{
		Asset * asset = pendingAsset.asset;
		if (!asset->vertices.data)
			continue;
		_createVertexBuffer(pendingAsset);
		if (!asset->indices.empty())
			_createIndexBuffer(pendingAsset);
		vk::DeviceSize vertexSize = asset->vertices.size;
		memcpy(static_cast<uint8_t *>(dataStaging) + _currentStagingBufferOffset, asset->vertices.data, vertexSize);
		_transferCommandBuffer.copyBuffer(_stagingBuffer.buffer, pendingAsset.vertexData.buffer,
			vk::BufferCopy(_currentStagingBufferOffset, 0, vertexSize));
		_currentStagingBufferOffset += alignTo(vertexSize, 16);
		if (!asset->indices.empty())
		{
			vk::DeviceSize indicesSize = sizeof(uint32_t) * asset->indices.size();
			memcpy(static_cast<uint8_t *>(dataStaging) + _currentStagingBufferOffset, asset->indices.data(), indicesSize);
			_transferCommandBuffer.copyBuffer(_stagingBuffer.buffer, pendingAsset.indexData.buffer,
				vk::BufferCopy(_currentStagingBufferOffset, 0, indicesSize));
			_currentStagingBufferOffset += alignTo(indicesSize, 16);
		}
	}
	_stagingBuffer.memory.unmapMemory();
	_transferCommandBuffer.end();
	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &*_transferCommandBuffer;
	_transferQueue.submit(submitInfo);
	_transferQueue.waitIdle();
	_currentStagingBufferOffset = 0;
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
	if (_pendingUnloads[_currentFrame].empty())
		return;
	for (AssetID assetID : _pendingUnloads[_currentFrame])
		_assetDataCache.erase(assetID);
	_pendingUnloads[_currentFrame].clear();
}
