#pragma once

#define VK_USE_PLATFORM_XCB_KHR
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include "AEngine.hpp"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#define VULKAN_CALLBACK VKAPI_ATTR vk::Bool32 VKAPI_CALL
#define DEBUG_LEVEL vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
#define MAX_FRAMES_IN_FLIGHT 2

const std::vector<const char *> g_validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

// La swapchain servira à présenter des images à la fenêtre
// Les autres ajoutent des fonctionnalités supplémentaires (?)
const std::vector<const char *> g_deviceExtensions =
{
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName,
	vk::KHRShaderDrawParametersExtensionName,
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

struct TransitionImageViewLayoutInfo
{
	uint32_t				imageIndex;
	vk::ImageLayout			oldLayout;
	vk::ImageLayout			newLayout;
	vk::AccessFlags2		srcAccessMask;
	vk::AccessFlags2		dstAccessMask;
	vk::PipelineStageFlags2	srcStageMask;
	vk::PipelineStageFlags2	dstStageMask;
};

class VulkanEngine : public AEngine
{
	public:
		VulkanEngine(GLFWwindow * window, Camera * camera);
		~VulkanEngine();

		void								load() override;
		void								drawFrame() override;

	private:
		typedef std::vector<char const *>							RequiredExtensions;
		typedef std::vector<char const *>							RequiredLayers;
		typedef std::vector<vk::raii::CommandBuffer>				CommandBuffers;
		typedef std::vector<vk::raii::Semaphore>					Semaphores;
		typedef std::vector<vk::raii::Fence>						Fences;
		typedef std::vector<vk::raii::DescriptorSet>				DescriptorSets;
		typedef std::array<vk::VertexInputAttributeDescription, 3>	VertexAttributeDescriptionArray;

		// Window, context, instance
		vk::raii::Context					_context;
		vk::raii::Instance					_instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT	_debugMessenger = nullptr;
		vk::raii::SurfaceKHR				_surface = nullptr;

		// Physical/Logical device
		vk::raii::PhysicalDevice			_physicalDevice = nullptr;
		vk::raii::Device					_device = nullptr;
		vk::raii::Queue						_queue = nullptr;
		QueueIndices						_queueIndices;

		// Swapchain & Images
		vk::raii::SwapchainKHR				_swapChain = nullptr;
		vk::SurfaceFormatKHR				_swapChainSurfaceFormat;
		vk::Extent2D						_swapChainExtent;
		vk::Format							_swapChainImageFormat;
		std::vector<vk::Image>				_swapChainImages;
		std::vector<vk::raii::ImageView>	_swapChainImageViews;
	
		// Pipeline & Descriptor
		vk::raii::DescriptorSetLayout		_descriptorSetLayout = nullptr;
		vk::raii::PipelineLayout			_pipelineLayout = nullptr;
		vk::raii::Pipeline					_graphicsPipeline = nullptr;
		vk::raii::CommandPool				_resetCommandPool = nullptr;
		vk::raii::CommandPool				_transientCommandPool = nullptr;
		CommandBuffers						_commandBuffers;

		// Sync primitives
		Semaphores							_presentCompleteSemaphores;
		Semaphores							_renderFinishedSemaphores;
		Fences								_inFlightFences;
		uint32_t							_semaphoreIndex = 0;
		uint32_t							_currentFrame = 0;

		// Buffers & Memory
		vk::raii::Buffer					_vertexBuffer = nullptr;
		uint32_t							_vertexSize;
		vk::raii::DeviceMemory				_vertexBufferMemory = nullptr;
		vk::raii::Buffer					_indexBuffer = nullptr;
		uint32_t							_indexSize;
		vk::raii::DeviceMemory				_indexBufferMemory = nullptr;
		std::vector<vk::raii::Buffer>		_uniformBuffers;
		std::vector<vk::raii::DeviceMemory>	_uniformBuffersMemory;
		std::vector<void *>					_uniformBuffersMapped;
		vk::raii::DescriptorPool			_descriptorPool = nullptr;
		DescriptorSets						_descriptorSets;

		// Textures
		vk::raii::Image						_textureImage = nullptr;
		vk::raii::DeviceMemory				_textureImageMemory = nullptr;
		vk::raii::ImageView					_textureImageView = nullptr;
		vk::raii::Sampler					_textureSampler = nullptr;

		// Depth test
		vk::raii::Image						_depthImage = nullptr;
		vk::raii::DeviceMemory				_depthImageMemory = nullptr;
		vk::raii::ImageView					_depthImageView = nullptr;

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
		void								_createCommandPool(vk::raii::CommandPool & commandPool, vk::CommandPoolCreateFlagBits flag);
		void								_createTextureImage();
		void								_createVertexBuffer(ModelType type);
		void								_createIndexBuffer(ModelType type);
		void								_createCommandBuffer();
		void								_recordCommandBuffer(uint32_t imageIndex);
		void								_transitionImageViewLayout(TransitionImageViewLayoutInfo info);
		void								_transitionImageLayout(const vk::raii::Image & image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
		void								_createSyncObjects();
		void								_recreateSwapchain();
		uint32_t							_findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
		void 								_createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer & buffer, vk::raii::DeviceMemory & deviceMemory);
		void								_copyBuffer(vk::raii::Buffer & srcBuffer, vk::raii::Buffer & dstBuffer, vk::DeviceSize size);
		vk::raii::CommandBuffer				_beginSingleTimeCommands();
		void								_endSingleTimeCommands(vk::raii::CommandBuffer & commandBuffer);
		vk::VertexInputBindingDescription	_getBindingDescription() const;
		VertexAttributeDescriptionArray		_getAttributeDescription() const;
		void								_createDescriptorSetLayout();
		void								_createUniformBuffers();
		void								_createDescriptorPool();
		void								_updateUniformBuffer();
		void								_createDescriptorSets();
		void								_createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image & image, vk::raii::DeviceMemory & imageMemory);
		void								_createTextureImageView();
		void								_copyBufferToImage(const vk::raii::Buffer & buffer, vk::raii::Image & image, uint32_t width, uint32_t height);
		void								_createTextureSampler();
		void								_createDepthResources();
		vk::Format							_findSupportedFormat(const std::vector<vk::Format> & candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
		vk::Format							_findDepthFormat();
};
