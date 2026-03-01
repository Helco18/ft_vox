#include "VulkanEngine.hpp"
#include "Logger.hpp"

vk::raii::CommandPool VulkanEngine::_createCommandPool(uint32_t queueIndex, vk::CommandPoolCreateFlags flags)
{
	// La command pool va stocker nos command buffers
	vk::CommandPoolCreateInfo commandPoolInfo;
	// On veut record nos commandes à chaque frame donc on reset la pool et re-record avec ce flag
	// L'autre flag est pour les commandes à courtes durée de vie (Transient)
	commandPoolInfo.flags = flags;
	commandPoolInfo.queueFamilyIndex = queueIndex;

	return vk::raii::CommandPool(_device, commandPoolInfo);
}

VulkanEngine::CommandBuffers VulkanEngine::_createCommandBuffers(vk::raii::CommandPool & commandPool, vk::CommandBufferLevel level, uint8_t count)
{
	CommandBuffers commandBuffers;

	// Les command buffers sont des instructions que l'on va donner au GPU, comme celle de draw.
	vk::CommandBufferAllocateInfo commandBufferInfo;
	commandBufferInfo.commandPool = commandPool;
	// Primaire : Peut être submit à la queue pour être exécutée
	// Secondaire : Ne peut pas être submit directement et doit être appelée par une primaire
	commandBufferInfo.level = level;
	commandBufferInfo.commandBufferCount = count;

	commandBuffers = vk::raii::CommandBuffers(_device, commandBufferInfo);

	return commandBuffers;
}

void VulkanEngine::_recordCommandBuffer()
{
	for (size_t i = 0; i < _drawableAssets.size(); ++i)
	{
		PipelineID pipelineID = i;
		PipelineData & pipelineData = _pipelineCache[pipelineID];
		if (pipelineData.pipeline == nullptr)
		{
			Logger::log(ENGINE_VULKAN, WARNING, "Tried recording command buffer on unknown pipeline");
			continue;
		}
		vk::CommandBufferBeginInfo commandBufferBeginInfo;
		vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;
		vk::CommandBuffer commands = pipelineData.commandBuffers[_currentFrame];
		commands.begin(commandBufferBeginInfo);

		vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

		vk::RenderingAttachmentInfo colorAttachmentInfo;
		colorAttachmentInfo.imageView = _msaaImageView;
		// colorAttachmentInfo.imageView = _swapChainImageViews[_imageIndex];
		// On précise son layout actuel (comme celui au-dessus)
		colorAttachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
		// Que faire de ce qu'il y a actuellement sur l'image avant de dessiner ?
		// Clear : Efface avec la clearColor
		// Load : Garder ce qu'il y a actuellement
		// DontCare : Ecrase le contenu
		colorAttachmentInfo.loadOp = !i ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
		// Que faire après le rendu ?
		// Store : On conserve pour l'afficher ensuite à l'écran
		// DontCare : On ne garde pas le résultat
		colorAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachmentInfo.clearValue = clearColor;
		colorAttachmentInfo.resolveMode = vk::ResolveModeFlagBits::eAverage;
		colorAttachmentInfo.resolveImageView = _swapChainImageViews[_imageIndex];
		colorAttachmentInfo.resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::RenderingInfo renderingInfo;
		vk::RenderingAttachmentInfo depthAttachmentInfo;
		vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0.0f);
		depthAttachmentInfo.imageView = _depthImageView;
		depthAttachmentInfo.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
		depthAttachmentInfo.loadOp = !i ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
		depthAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
		depthAttachmentInfo.clearValue = clearDepth;
		renderingInfo.pDepthAttachment = &depthAttachmentInfo;

		renderingInfo.renderArea.offset.x = 0;
		renderingInfo.renderArea.offset.y = 0;
		renderingInfo.renderArea.extent = _swapChainExtent;
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachmentInfo;

		commands.beginRendering(renderingInfo);
		commands.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(_swapChainExtent.width),
						static_cast<float>(_swapChainExtent.height), 0.0f, 1.0f));
		commands.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), _swapChainExtent));
		commands.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelineData.pipeline);
		if (!pipelineData.pipelineInfo.descriptors.empty())
		{
			commands.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineData.layout, 0,
					*pipelineData.descriptorSets[_currentFrame], nullptr);
		}
		const std::vector<Asset *> & drawableAssets = _drawableAssets[i];
		for (const Asset * asset : drawableAssets)
		{
			const AssetData & assetData = _assetDataCache[asset->assetID];
			const BufferData & vertexData = assetData.vbo;
			const BufferData & indexData = assetData.ibo;
			if (asset->uniforms != nullptr)
			{
				commands.pushConstants(pipelineData.layout, vk::ShaderStageFlagBits::eAll, 0, pipelineData.pipelineInfo.uniformSize,
					asset->uniforms);
			}
			commands.bindVertexBuffers(0, *vertexData.buffer, {0});
			if (indexData.buffer != nullptr)
			{
				commands.bindIndexBuffer(*indexData.buffer, 0, vk::IndexType::eUint32);
				commands.drawIndexed(asset->indices.size(), 1, 0, 0, 0);
			}
			else
				commands.draw(asset->vertices.vertexCount, 1, 0, 0);
		}
		commands.endRendering();
		commands.end();
		_graphicsCommandBuffers[_currentFrame].executeCommands(commands);
	}
	_renderImGui();
}

void VulkanEngine::_retrieveCommandBuffers()
{
	_graphicsCommandBuffers[_currentFrame].reset();
	_processPendingAssets();
	_processPendingUniforms();
	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	_graphicsCommandBuffers[_currentFrame].begin(commandBufferBeginInfo);

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
	transitionImageViewInfo.srcStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe;
	// La barrière doit être validée avant que la pipeline atteigne la phase d'écriture du color attachment
	transitionImageViewInfo.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;

	// Transitioner le layout de l'image d'undefined à colorattachment dans notre cas
	_transitionImageViewLayout(transitionImageViewInfo);

	_recordCommandBuffer();

	TransitionImageViewLayoutInfo presentSrcInfo;
	presentSrcInfo.imageIndex = _imageIndex;
	presentSrcInfo.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
	presentSrcInfo.newLayout = vk::ImageLayout::ePresentSrcKHR;
	presentSrcInfo.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
	presentSrcInfo.dstAccessMask = {};
	presentSrcInfo.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
	presentSrcInfo.dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe;

	_transitionImageViewLayout(presentSrcInfo);
	_graphicsCommandBuffers[_currentFrame].end();
}

vk::raii::CommandBuffer VulkanEngine::_beginSingleTimeCommands()
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.commandPool = _graphicsCommandPool;
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
	_graphicsQueue.submit(submitInfo);
	_graphicsQueue.waitIdle();
}
