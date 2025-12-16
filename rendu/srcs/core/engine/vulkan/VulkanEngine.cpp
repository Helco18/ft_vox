#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"
#include "Logger.hpp"
#include <iostream>

VulkanEngine::VulkanEngine(GLFWwindow * window, Camera * camera, bool isWireframeEnabled) : AEngine(window, camera, isWireframeEnabled) {}

void VulkanEngine::load()
{
	// On initialise une instance Vulkan, qui représente notre application au niveau de l'API Vulkan.
	// C'est le point d'entrée principal entre l'application et le driver Vulkan (on y précise nom, version, extensions, etc.)
	_createInstance();

	// On initialise le debug pour capturer les erreurs, avertissements et validations pendant le développement.
	_initDebugMessenger();

	// On créé la surface qui relie Vulkan à la fenêtre GLFW.
	// C’est via cette surface que Vulkan saura où présenter les images.
	_createSurface();

	// On choisit la carte graphique compatible avec Vulkan et ses extensions à utiliser
	_selectPhysicalDevice();

	// On regarde si notre machine est compatible avec les extensions Vulkan dont on a besoin
	// (par exemple VK_KHR_swapchain, nécessaire pour l’affichage).
	_checkDeviceExtensions();

	// On crée un *logical device*, c’est-à-dire une interface logique vers le GPU physique sélectionné.
	// On active les queue families (graphics, presentation, ...) et on active les features de Vulkan que l'on veut.
	_createLogicalDevice();

	// On crée une *swapchain* : c’est un ensemble d’images que Vulkan utilisera pour afficher les rendus à l’écran.
	// Elle permet d’alterner entre plusieurs buffers (double/triple buffering).
	_createSwapChain();

	// On crée des *image views* à partir des images de la swapchain.
	// Chaque image view décrit comment une image doit être interprétée (format, aspect, niveau de mipmap, etc.).
	_createImageViews();

	// On définit la structure de nos *descriptor sets*, qui servent à passer des données aux shaders
	// (comme les uniform buffers, textures ou samplers).
	_createDescriptorSetLayout();

	// On construit notre *graphics pipeline*, c’est-à-dire la configuration complète du pipeline graphique :
	// shaders, entrées vertex, assemblage, rasterization, blending, etc.
	_createGraphicsPipelines();

	// On crée deux *command pools* :
	// - un "reset" pool pour des commandes réutilisables (e.g. dessin, rendu)
	// - un "transient" pool pour des commandes temporaires (e.g. copie de buffer, transfert de ressources)
	_createCommandPool(_resetCommandPool, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	_createCommandPool(_transientCommandPool, vk::CommandPoolCreateFlagBits::eTransient);

	// On crée les buffers pour stocker les données géométriques :
	// - vertex buffer : contient les positions, couleurs, normales, etc.
	// - index buffer : décrit l’ordre dans lequel les sommets sont connectés.
	// - uniform buffer : ils contiennent les données variables entre les frames (comme les matrices de transformation / constantes globales utilisées par les shaders)
	_createDepthResources();
	_createTextureImage();
	_createTextureImageView();
	_createTextureSampler();

	_createUniformBuffers();

	_createDescriptorPool();
	_createDescriptorSets();

	// On enregistre dans un *command buffer* toutes les commandes Vulkan nécessaires pour dessiner nos objets.
	// Ce buffer sera soumis à une file de commandes à chaque frame.
	_createCommandBuffer();

	// On crée les *semaphores* et *fences* pour la synchronisation :
	// ils permettent de s’assurer que les opérations GPU (rendu, présentation, etc.) s’exécutent dans le bon ordre et ne se chevauchent pas.
	_createSyncObjects();

	_isInitalized = true;
	Logger::log(ENGINE_VULKAN, INFO, "Vulkan engine initialized successfully.");
}

VulkanEngine::~VulkanEngine()
{
	_device.waitIdle();
	_queue.waitIdle();
}

void VulkanEngine::beginFrame()
{
	_drawableAssets.clear();
}

AssetID VulkanEngine::uploadAsset(Asset & asset)
{
	static AssetID assetID = 1;

	asset.vbo = _vertexSize;
	asset.ibo = _indexSize;
	_concateneVertexBuffer(asset);
	_concateneIndexBuffer(asset);
	asset.assetID = assetID;

	_assetMap.try_emplace(assetID, asset);
	return assetID++;
}

void VulkanEngine::drawAsset(AssetID assetID)
{
	_drawableAssets.push_back(&_assetMap[assetID]);
}

void VulkanEngine::endFrame()
{
	while (vk::Result::eTimeout == _device.waitForFences(*_inFlightFences[_currentFrame], vk::True, std::numeric_limits<uint64_t>::max()))
		;

	// Semaphore = on ordonne les tâches
	// Fence = on attend que le GPU finisse la tâche
	try
	{
		std::pair<vk::Result, uint32_t> result = _swapChain.acquireNextImage(std::numeric_limits<uint64_t>::max(), *_presentCompleteSemaphores[_presentSemaphoreIndex], nullptr);
		_imageIndex = result.second;
		if (result.first != vk::Result::eSuccess && result.first != vk::Result::eSuboptimalKHR)
		{
			if (result.first == vk::Result::eErrorOutOfDateKHR)
			{
				_recreateSwapchain();
				return;
			}
			throw VulkanException("Couldn't acquire next image.");
		}
		_device.resetFences(*_inFlightFences[_currentFrame]);

		_updateUniformBuffer();

		_commandBuffers[_currentFrame].reset();
		_recordCommandBuffer();

		vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		vk::SubmitInfo submitInfo;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &*_presentCompleteSemaphores[_presentSemaphoreIndex];
		submitInfo.pWaitDstStageMask = &waitDstStageMask;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &*_commandBuffers[_currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &*_renderFinishedSemaphores[_imageIndex];

		_queue.submit(submitInfo, *_inFlightFences[_currentFrame]);

		vk::PresentInfoKHR presentInfoKHR;
		presentInfoKHR.waitSemaphoreCount = 1;
		presentInfoKHR.pWaitSemaphores = &*_renderFinishedSemaphores[_imageIndex];
		presentInfoKHR.swapchainCount = 1;
		presentInfoKHR.pSwapchains = &*_swapChain;
		presentInfoKHR.pImageIndices = &_imageIndex;

		vk::Result presentResult = _queue.presentKHR(presentInfoKHR);
		if (presentResult != vk::Result::eSuccess)
		{
			if (presentResult == vk::Result::eSuboptimalKHR)
			{
				_isFramebufferResized = false;
				_recreateSwapchain();
			}
			else
				throw VulkanException("Couldn't present next image.");
		}

		while (vk::Result::eTimeout == _device.waitForFences(*_inFlightFences[_currentFrame], vk::True, std::numeric_limits<uint64_t>::max()))
			;
		_commandBuffers[_currentFrame].reset();
		_presentSemaphoreIndex = (_presentSemaphoreIndex + 1) % _presentCompleteSemaphores.size();
		_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	} catch (const vk::OutOfDateKHRError & e)
	{
		_isFramebufferResized = false;
		_recreateSwapchain();
	}
}
