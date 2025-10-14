#pragma once

#define VK_USE_PLATFORM_XCB_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
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
    vk::KHRCreateRenderpass2ExtensionName,
	vk::KHRShaderDrawParametersExtensionName
};

constexpr bool g_enableValidationLayers = true;

// #ifdef DEBUG
// constexpr bool g_enableValidationLayers = true;
// #else
// constexpr bool g_enableValidationLayers = false;
// #endif

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


struct Vertex
{
	glm::vec2 position;
	glm::vec3 color;

	static vk::VertexInputBindingDescription getBindingDescription()
	{
		// Layout id, stride (taille de ta donnée), vertex ou instance
		return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
	}

	static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescription()
	{
		// Spécifier ce que notre vertex a comme attributs. Ici, nous avons sa position dans la location 0 et sa couleur dans la location 1.
		// On stocke des coordonnées en couleur.
		return {
			vk::VertexInputAttributeDescription( 0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)),
			vk::VertexInputAttributeDescription( 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color))
		};
	}
};

class VulkanEngine
{
	public:
		VulkanEngine(GLFWwindow * window);
		~VulkanEngine();

		void								drawFrame();

		const vk::raii::Device &			getDevice() const { return _device; };
		void								waitIdle();
		static void							framebufferResizeCallback(GLFWwindow * window, int width, int height);
		static std::vector<Vertex>			getVertexFromFile(const std::string & path);

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
		uint32_t							_semaphoreIndex = 0;
		uint32_t							_currentFrame = 0;
		bool								_framebufferResized = false;
		vk::raii::Buffer					_vertexBuffer = nullptr;
		uint32_t							_vertexSize;
		vk::raii::DeviceMemory				_vertexBufferMemory = nullptr;

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
		void								_createVertexBuffer(const std::string & modelpath);
		void								_createCommandBuffer();
		void								_recordCommandBuffer(uint32_t imageIndex);
		void								_transitionImageLayout(TransitionImageLayoutInfo info);
		void								_createSyncObjects();
		void								_recreateSwapchain();
		uint32_t							_findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
};
