#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"
#include "Logger.hpp"

VulkanEngine::VulkanEngine(GLFWwindow * window) : AEngine(window)
{
	_engineType = VULKAN;
	// _threadPool.start(2);
}

VulkanEngine::~VulkanEngine()
{
	_isInitalized.store(false);
	_device.waitIdle();
	_shutdownImGui();
	// _threadPool.stop();
}

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

	// On crée un *logical device*, c’est-à-dire une interface logique vers le GPU physique sélectionné.
	// On active les queue families (graphics, presentation, ...) et on active les features de Vulkan que l'on veut.
	_createLogicalDevice();

	// On crée une *swapchain* : c’est un ensemble d’images que Vulkan utilisera pour afficher les rendus à l’écran.
	// Elle permet d’alterner entre plusieurs buffers (double/triple buffering).
	_createSwapChain();

	// On crée des *image views* à partir des images de la swapchain.
	// Chaque image view décrit comment une image doit être interprétée (format, aspect, niveau de mipmap, etc.).
	_createImageViews();

	// On crée un *command pool* avec les flags :
	// - "reset" pour avoir la possibilité de reset chaque buffer individuellement (sinon on ne pourrait que reset la pool entière)
	// - "transient" pour optimiser les commandes qui vont etre fréquemment reset
	_graphicsCommandPool = _createCommandPool(_queueIndices.graphicsIndex, vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient);
	_transferCommandPool = _createCommandPool(_queueIndices.transferIndex, vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient);

	_graphicsCommandBuffers = _createCommandBuffers(_graphicsCommandPool, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT);
	_transferCommandBuffer = std::move(_createCommandBuffers(_transferCommandPool, vk::CommandBufferLevel::ePrimary, 1)[0]);

	_createMultisamplingImage();
	// On crée les buffers pour stocker les données géométriques :
	// - vertex buffer : contient les positions, couleurs, normales, etc.
	// - index buffer : décrit l’ordre dans lequel les sommets sont connectés.
	// - uniform buffer : ils contiennent les données variables entre les frames (comme les matrices de transformation / constantes globales utilisées par les shaders)
	_createDepthResources();

	// On crée les *semaphores* et *fences* pour la synchronisation :
	// ils permettent de s’assurer que les opérations GPU (rendu, présentation, etc.) s’exécutent dans le bon ordre et ne se chevauchent pas.
	_createSyncObjects();

	// On construit notre *graphics pipeline*, c’est-à-dire la configuration complète du pipeline graphique :
	// shaders, entrées vertex, assemblage, rasterization, blending, etc.

	_initImGui();
	_isInitalized = true;
	Logger::log(ENGINE_VULKAN, INFO, "Vulkan engine initialized successfully.");
}

void VulkanEngine::setVsync(bool vsync)
{
	if (_vsync != vsync)
	{
		_vsync = vsync;
		_recreateSwapchain();
	}
}

void VulkanEngine::beginFrame()
{
	_drawableAssets.clear();
	if (_drawableAssets.size() != _pipelineCache.size())
		_drawableAssets.resize(_pipelineCache.size());
}

void VulkanEngine::endFrame()
{
	if (_drawableAssets.empty())
		return;
	// Semaphore = on ordonne les tâches
	// Fence = on attend que le GPU finisse la tâche
	try
	{
		vk::Result res = _device.waitForFences(*_inFlightFences[_currentFrame], vk::True, UINT64_MAX);
		if (res != vk::Result::eSuccess)
			throw VulkanException("Waiting for fences on draw call failed.");

		std::pair<vk::Result, uint32_t> result = _swapChain.acquireNextImage(UINT64_MAX, *_presentCompleteSemaphores[_currentFrame], nullptr);
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

		_retrieveCommandBuffers();

		vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		vk::SubmitInfo submitInfo;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &*_presentCompleteSemaphores[_currentFrame];
		submitInfo.pWaitDstStageMask = &waitDstStageMask;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &*_graphicsCommandBuffers[_currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &*_renderFinishedSemaphores[_imageIndex];

		_graphicsQueue.submit(submitInfo, *_inFlightFences[_currentFrame]);

		vk::PresentInfoKHR presentInfoKHR;
		presentInfoKHR.waitSemaphoreCount = 1;
		presentInfoKHR.pWaitSemaphores = &*_renderFinishedSemaphores[_imageIndex];
		presentInfoKHR.swapchainCount = 1;
		presentInfoKHR.pSwapchains = &*_swapChain;
		presentInfoKHR.pImageIndices = &_imageIndex;

		vk::Result presentResult = _graphicsQueue.presentKHR(presentInfoKHR);
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

		_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	} catch (const vk::OutOfDateKHRError & e)
	{
		_isFramebufferResized = false;
		_recreateSwapchain();
	}
}
