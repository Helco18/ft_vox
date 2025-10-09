#include "VulkanEngine.hpp"

void VulkanEngine::_createCommandPool()
{
	// La command pool va stocker nos command buffers
	vk::CommandPoolCreateInfo commandPoolInfo;
	// On veut record nos commandes à chaque frame donc on reset la pool et re-record avec ce flag
	// L'autre flag est pour les commandes à courtes durée de vie (Transient)
	commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	commandPoolInfo.queueFamilyIndex = _queueIndices.graphicsIndex;

	_commandPool = vk::raii::CommandPool(_device, commandPoolInfo);

	if (g_enableValidationLayers)
		std::cout << GREEN << "[OK] Created Command Pool" << std::endl;
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
		std::cout << GREEN << "[OK] Created Command Buffer" << std::endl;
}

void VulkanEngine::_transitionImageLayout(TransitionImageLayoutInfo info)
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

void VulkanEngine::_recordCommandBuffer(uint32_t imageIndex)
{
	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	_commandBuffers[_currentFrame].begin(commandBufferBeginInfo);

	TransitionImageLayoutInfo colorAttachmentInfo;
	colorAttachmentInfo.imageIndex = imageIndex;
	// Un ImageLayout est l'état de l'image.
	// Undefined : Vient d'être créée
	// ColorAttachmentOptimal : Utilisée comme cible de rendu (render target)
	// PresentSrcKHR : Prête à être affichée à l'écran
	// TransferDstOptimal : Prête à recevoir un transfert de données (un upload de texture par exemple)
	colorAttachmentInfo.oldLayout = vk::ImageLayout::eUndefined;
	colorAttachmentInfo.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
	// Il n'y a pas d'opération à attendre avant, l'image est undefined donc non-utilisée
	colorAttachmentInfo.srcAccessMask = {};
	// Après la barrière, on veut écrire dans cette image (on écrit les pixels du framebuffer)
	colorAttachmentInfo.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
	// La barrière doit se placer avant toute opération de la pipeline
	colorAttachmentInfo.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
	// La barrière doit être validée avant que la pipeline atteigne la phase d'écriture du color attachment
	colorAttachmentInfo.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;

	// Transitioner le layout de l'image d'undefined à colorattachment dans notre cas
	_transitionImageLayout(colorAttachmentInfo);

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::RenderingAttachmentInfo attachmentInfo;
	attachmentInfo.imageView = _swapChainImageViews[imageIndex];
	// On précise son layout actuel (comme celui au-dessus)
	attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
	// Que faire de ce qu'il y a actuellement sur l'image avant de dessiner ?
	// Clear : Efface avec la clearColor
	// Load : Garder ce qu'il y a actuellement
	// DontCare : Ecrase le contenu
	attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
	// Que faire après le rendu ?
	// Store : On conserve pour l'afficher ensuite à l'écran
	// DontCare : On ne garde pas le résultat
	attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
	attachmentInfo.clearValue = clearColor;

	vk::RenderingInfo renderingInfo;
	renderingInfo.renderArea.offset.x = 0;
	renderingInfo.renderArea.offset.y = 0;
	renderingInfo.renderArea.extent = _swapChainExtent;
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &attachmentInfo;

	_commandBuffers[_currentFrame].beginRendering(renderingInfo);
	_commandBuffers[_currentFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, _graphicsPipeline);
	_commandBuffers[_currentFrame].setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(_swapChainExtent.width), static_cast<float>(_swapChainExtent.height), 0.0f, 1.0f));
	_commandBuffers[_currentFrame].setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), _swapChainExtent));
	_commandBuffers[_currentFrame].draw(3, 1, 0, 0);
	_commandBuffers[_currentFrame].endRendering();

	TransitionImageLayoutInfo presentSrcInfo;
	presentSrcInfo.imageIndex = imageIndex;
	presentSrcInfo.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
	presentSrcInfo.newLayout = vk::ImageLayout::ePresentSrcKHR;
	presentSrcInfo.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
	presentSrcInfo.dstAccessMask = {};
	presentSrcInfo.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
	presentSrcInfo.dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe;

	_transitionImageLayout(presentSrcInfo);
	_commandBuffers[_currentFrame].end();
}

void VulkanEngine::_createSyncObjects()
{
	_presentCompleteSemaphores.clear();
	_renderFinishedSemaphores.clear();
	_inFlightFences.clear();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		_presentCompleteSemaphores.emplace_back(vk::raii::Semaphore(_device, vk::SemaphoreCreateInfo()));
		_renderFinishedSemaphores.emplace_back(vk::raii::Semaphore(_device, vk::SemaphoreCreateInfo()));

		vk::FenceCreateInfo fenceInfo;
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
		_inFlightFences.emplace_back(vk::raii::Fence(_device, fenceInfo));
	}

	if (g_enableValidationLayers)
		std::cout << GREEN << "[OK] Created Sync Objects" << std::endl;
}
