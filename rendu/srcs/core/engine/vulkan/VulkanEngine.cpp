#include "VulkanEngine.hpp"
#include "CustomExceptions.hpp"
#include "Logger.hpp"
#include "Profiler.hpp"
#include "utils.hpp"
#include <iostream>

VulkanEngine::VulkanEngine(GLFWwindow * window) : AEngine(window) { _engineType = VULKAN; }

VulkanEngine::~VulkanEngine()
{
	_device.waitIdle();
	_queue.waitIdle();
}

void VulkanEngine::load()
{
	Profiler p("VulkanEngine::load");

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
	_createCommandPool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient);

	// On enregistre dans un *command buffer* toutes les commandes Vulkan nécessaires pour dessiner nos objets.
	// Ce buffer sera soumis à une file de commandes à chaque frame.
	_createCommandBuffer();

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

	_isInitalized = true;
	Logger::log(ENGINE_VULKAN, INFO, "Vulkan engine initialized successfully.");
}

void VulkanEngine::beginFrame()
{
	for (std::pair<const PipelineID, std::vector<Asset *>> & pipelinePair : _pipelineAssetMap)
		pipelinePair.second.clear();
}

void VulkanEngine::endFrame()
{
	// Semaphore = on ordonne les tâches
	// Fence = on attend que le GPU finisse la tâche
	try
	{
		std::pair<vk::Result, uint32_t> result = _swapChain.acquireNextImage(UINT64_MAX, *_presentCompleteSemaphores[_presentSemaphoreIndex], nullptr);
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
		
		vk::Result res = _device.waitForFences(*_inFlightFences[_currentFrame], vk::True, UINT64_MAX);
		if (res != vk::Result::eSuccess)
			throw VulkanException("Waiting for fences on draw call failed.");
		_device.resetFences(*_inFlightFences[_currentFrame]);

		_commandBuffers[_currentFrame].reset();
		_uploadPendingAssets();
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

		_presentSemaphoreIndex = (_presentSemaphoreIndex + 1) % _presentCompleteSemaphores.size();
		_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	} catch (const vk::OutOfDateKHRError & e)
	{
		_isFramebufferResized = false;
		_recreateSwapchain();
	}
}
