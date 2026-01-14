#include "VulkanEngine.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <iostream>

void VulkanEngine::_createCommandPool(vk::CommandPoolCreateFlags flags)
{
	// La command pool va stocker nos command buffers
	vk::CommandPoolCreateInfo commandPoolInfo;
	// On veut record nos commandes à chaque frame donc on reset la pool et re-record avec ce flag
	// L'autre flag est pour les commandes à courtes durée de vie (Transient)
	commandPoolInfo.flags = flags;
	commandPoolInfo.queueFamilyIndex = _queueIndices.graphicsIndex;

	_commandPool = vk::raii::CommandPool(_device, commandPoolInfo);

	if (g_enableValidationLayers)
		Logger::log(ENGINE_VULKAN, INFO, "Created Command Pool.");
}

void VulkanEngine::_createCommandBuffer()
{
	_commandBuffers.clear();

	// Les command buffers sont des instructions que l'on va donner au GPU, comme celle de draw.
	vk::CommandBufferAllocateInfo commandBufferInfo;
	commandBufferInfo.commandPool = _commandPool;
	// Primaire : Peut être submit à la queue pour être exécutée
	// Secondaire : Ne peut pas être submit directement et doit être appelée par une primaire
	commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
	commandBufferInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

	_commandBuffers = vk::raii::CommandBuffers(_device, commandBufferInfo);

	if (g_enableValidationLayers)
		Logger::log(ENGINE_VULKAN, INFO, "Created Command Buffer.");
}

void VulkanEngine::_transitionImageViewLayout(TransitionImageViewLayoutInfo info)
{
	// Une barrière est comme un mutex, elle attend que l'image soit disponible puis change de layout
	vk::ImageMemoryBarrier2 barrier;
	barrier.oldLayout = info.oldLayout;
	barrier.newLayout = info.newLayout;
	barrier.srcAccessMask = info.srcAccessMask;
	barrier.dstAccessMask = info.dstAccessMask;
	barrier.srcStageMask = info.srcStageMask;
	barrier.dstStageMask = info.dstStageMask;
	barrier.image = _swapChainImages[info.imageIndex];
	// On change ici si on veut changer la queue family de l'image. Dans la plupart des cas, on ignore pour ne pas changer.
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	// Une Dependency info regroupe plusieurs types de barrière, ici on en a qu'une donc on la lui donne
	vk::DependencyInfo dependencyInfo;
	dependencyInfo.dependencyFlags = {};
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &barrier;

	_commandBuffers[_currentFrame].pipelineBarrier2(dependencyInfo);
}

void VulkanEngine::_recordCommandBuffer()
{
	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	vk::CommandBuffer commands = _commandBuffers[_currentFrame];
	commands.begin(commandBufferBeginInfo);

	TransitionImageViewLayoutInfo transitionImageViewInfo;
	transitionImageViewInfo.imageIndex = _imageIndex;
	// Un ImageLayout est l'état de l'image.
	// Undefined : Vient d'être créée
	// ColorAttachmentOptimal : Utilisée comme cible de rendu (render target)
	// PresentSrcKHR : Prête à être affichée à l'écran
	// TransferDstOptimal : Prête à recevoir un transfert de données (un upload de texture par exemple)
	transitionImageViewInfo.oldLayout = vk::ImageLayout::eUndefined;
	transitionImageViewInfo.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
	// Il n'y a pas d'opération à attendre avant, l'image est undefined donc non-utilisée
	transitionImageViewInfo.srcAccessMask = {};
	// Après la barrière, on veut écrire dans cette image (on écrit les pixels du framebuffer)
	transitionImageViewInfo.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
	// Pas besoin d'attendre un stage en particulier car les images sont nouvelles et n'ont donc pas encore été utilisées
	transitionImageViewInfo.srcStageMask = vk::PipelineStageFlagBits2::eNone;
	// La barrière doit être validée avant que la pipeline atteigne la phase d'écriture du color attachment
	transitionImageViewInfo.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;

	// Transitioner le layout de l'image d'undefined à colorattachment dans notre cas
	_transitionImageViewLayout(transitionImageViewInfo);

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0.0f);

	vk::RenderingAttachmentInfo colorAttachmentInfo;
	colorAttachmentInfo.imageView = _swapChainImageViews[_imageIndex];
	// On précise son layout actuel (comme celui au-dessus)
	colorAttachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
	// Que faire de ce qu'il y a actuellement sur l'image avant de dessiner ?
	// Clear : Efface avec la clearColor
	// Load : Garder ce qu'il y a actuellement
	// DontCare : Ecrase le contenu
	colorAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
	// Que faire après le rendu ?
	// Store : On conserve pour l'afficher ensuite à l'écran
	// DontCare : On ne garde pas le résultat
	colorAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachmentInfo.clearValue = clearColor;

	vk::RenderingAttachmentInfo depthAttachmentInfo;
	depthAttachmentInfo.imageView = _depthImageView;
	depthAttachmentInfo.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	depthAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
	depthAttachmentInfo.storeOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachmentInfo.clearValue = clearDepth;

	vk::RenderingInfo renderingInfo;
	renderingInfo.renderArea.offset.x = 0;
	renderingInfo.renderArea.offset.y = 0;
	renderingInfo.renderArea.extent = _swapChainExtent;
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachmentInfo;
	renderingInfo.pDepthAttachment = &depthAttachmentInfo;

	commands.beginRendering(renderingInfo);
	commands.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(_swapChainExtent.width),
					static_cast<float>(_swapChainExtent.height), 0.0f, 1.0f));
	commands.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), _swapChainExtent));
	for (const std::pair<const PipelineID, std::vector<Asset *>> & pipelineAsset : _pipelineAssetMap)
	{
		PipelineID pipelineID = pipelineAsset.first;
		PipelineMap::iterator pipelineit = _pipelineMap.find(pipelineID);
		if (pipelineit == _pipelineMap.end())
			continue;
		PipelineObjects & pipelineObjects = pipelineit->second;
		commands.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelineObjects.pipeline);
		commands.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineObjects.layout, 0,
				*_descriptorSets[_currentFrame], nullptr);
		const std::vector<Asset *> & drawableAssets = pipelineAsset.second;
		for (const Asset * asset : drawableAssets)
		{
			BufferCache::iterator vertexit = _vboCache.find(asset->assetID);
			BufferCache::iterator indexit = _iboCache.find(asset->assetID);
			if (vertexit == _vboCache.end() || indexit == _iboCache.end())
				continue;
			BufferData & vertexData = vertexit->second;
			BufferData & indexData = indexit->second;
			commands.bindVertexBuffers(0, *vertexData.buffer, {0});
			commands.bindIndexBuffer( *indexData.buffer, 0, vk::IndexType::eUint32);
			commands.drawIndexed(asset->indices.size(), 1, 0, 0, 0);
		}
	}
	commands.endRendering();

	TransitionImageViewLayoutInfo presentSrcInfo;
	presentSrcInfo.imageIndex = _imageIndex;
	presentSrcInfo.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
	presentSrcInfo.newLayout = vk::ImageLayout::ePresentSrcKHR;
	presentSrcInfo.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
	presentSrcInfo.dstAccessMask = {};
	presentSrcInfo.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
	presentSrcInfo.dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe;

	_transitionImageViewLayout(presentSrcInfo);
	commands.end();
}

void VulkanEngine::_createSyncObjects()
{
	_presentCompleteSemaphores.clear();
	_renderFinishedSemaphores.clear();
	_inFlightFences.clear();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vk::FenceCreateInfo fenceInfo;
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
		_inFlightFences.emplace_back(_device, fenceInfo);
		_presentCompleteSemaphores.emplace_back(_device, vk::SemaphoreCreateInfo());
	}
	for (size_t i = 0; i < _swapChainImages.size(); ++i)
	{
		_presentCompleteSemaphores.emplace_back(vk::raii::Semaphore(_device, vk::SemaphoreCreateInfo()));
		_renderFinishedSemaphores.emplace_back(vk::raii::Semaphore(_device, vk::SemaphoreCreateInfo()));
	}

	if (g_enableValidationLayers)
		Logger::log(ENGINE_VULKAN, INFO, "Created Sync Objects.");
}

vk::raii::CommandBuffer VulkanEngine::_beginSingleTimeCommands()
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.commandPool = _commandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	vk::raii::CommandBuffer commandCopyBuffer = std::move(_device.allocateCommandBuffers(allocInfo).front());

	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandCopyBuffer.begin(commandBufferBeginInfo);

	return commandCopyBuffer;
}

void VulkanEngine::_endSingleTimeCommands(vk::raii::CommandBuffer & commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &*commandBuffer;
	_queue.submit(submitInfo);
	_queue.waitIdle();
}
