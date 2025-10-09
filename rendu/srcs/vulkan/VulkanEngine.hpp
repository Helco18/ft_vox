#pragma once

#define VK_USE_PLATFORM_XCB_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <iterator>
#include <limits>
#include "colors.hpp"
#include "utils.hpp"

#define VULKAN_CALLBACK VKAPI_ATTR vk::Bool32 VKAPI_CALL
#define DEBUG_LEVEL vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
#define MAX_FRAMES_IN_FLIGHT 2

const std::vector g_validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

// La swapchain servira à présenter des images à la fenêtre
// Les autres ajoutent des fonctionnalités supplémentaires (?)
const std::vector<const char * > g_deviceExtensions =
{
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};

#ifdef DEBUG
constexpr bool g_enableValidationLayers = true;
#else
constexpr bool g_enableValidationLayers = false;
#endif

struct QueueIndices
{
	uint32_t	graphicsIndex;
	uint32_t	presentIndex;
};

struct TransitionImageLayoutInfo
{
	uint32_t				imageIndex;
	vk::ImageLayout			oldLayout;
	vk::ImageLayout			newLayout;
	vk::AccessFlags2		srcAccessMask;
	vk::AccessFlags2		dstAccessMask;
	vk::PipelineStageFlags2	srcStageMask;
	vk::PipelineStageFlags2	dstStageMask;
};

class VulkanEngine
{
	public:
		VulkanEngine(GLFWwindow * window);
		~VulkanEngine();

		void								drawFrame();

		const vk::raii::Device &			getDevice() const { return _device; };
		void								waitIdle();

	private:
		typedef std::vector<char const * >				RequiredExtensions;
		typedef std::vector<char const * >				RequiredLayers;
		typedef std::vector<vk::raii::CommandBuffer>	CommandBuffers;
		typedef std::vector<vk::raii::Semaphore>		Semaphores;
		typedef std::vector<vk::raii::Fence>			Fences;

		GLFWwindow *						_window;
		vk::raii::Context					_context;
		vk::raii::Instance					_instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT	_debugMessenger = nullptr;
		vk::raii::PhysicalDevice			_physicalDevice = nullptr;
		vk::raii::Device					_device = nullptr;
		vk::raii::Queue						_queue = nullptr;
		vk::raii::SurfaceKHR				_surface = nullptr;
		vk::SurfaceFormatKHR				_swapChainSurfaceFormat;
		vk::Extent2D						_swapChainExtent;
		QueueIndices						_queueIndices;
		vk::raii::SwapchainKHR				_swapChain = nullptr;
		std::vector<vk::Image>				_swapChainImages;
		vk::Format							_swapChainImageFormat;
		std::vector<vk::raii::ImageView>	_swapChainImageViews;
		vk::raii::PipelineLayout			_pipelineLayout = nullptr;
		vk::raii::Pipeline					_graphicsPipeline = nullptr;
		vk::raii::CommandPool				_commandPool = nullptr;
		CommandBuffers						_commandBuffers;
		Semaphores							_presentCompleteSemaphores;
		Semaphores							_renderFinishedSemaphores;
		Fences								_inFlightFences;
		uint32_t							_currentFrame = 0;

		void								_createInstance();
		void								_initDebugMessenger();
		void								_createSurface();
		RequiredExtensions					_getRequiredExtensions() const;
		RequiredLayers						_getRequiredLayers() const;
		void								_selectPhysicalDevice();
		QueueIndices						_findQueueFamilies() const;
		void								_createLogicalDevice();
		void								_checkDeviceExtensions() const;
		vk::SurfaceFormatKHR				_chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> & formats);
		vk::PresentModeKHR					_chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> & presentModes);
		vk::Extent2D						_chooseSwapExtent(const vk::SurfaceCapabilitiesKHR & capabilities);
		void								_createSwapChain();
		void								_createImageViews();
		void								_createGraphicsPipeline();
		vk::raii::ShaderModule				_createShaderModule(const std::vector<char> & shaderSrc) const;
		void								_createCommandPool();
		void								_createCommandBuffer();
		void								_recordCommandBuffer(uint32_t imageIndex);
		void								_transitionImageLayout(TransitionImageLayoutInfo info);
		void								_createSyncObjects();
};
