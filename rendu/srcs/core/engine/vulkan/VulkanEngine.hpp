#pragma once

#define VK_USE_PLATFORM_XCB_KHR
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include "AEngine.hpp"
#include "OBJModel.hpp"
#include <vulkan/vulkan_raii.hpp>

#define VULKAN_CALLBACK VKAPI_ATTR vk::Bool32 VKAPI_CALL
#define DEBUG_LEVEL vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
#define MAX_FRAMES_IN_FLIGHT 2
#define NEAR_PLANE_OFFSET 0.5f
#define VULKAN_SHADER_PATH "srcs/core/shaders/spir-v/"

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

struct TextureData
{
	vk::raii::Sampler		textureSampler = nullptr;
	vk::raii::ImageView		textureImageView = nullptr;
	vk::raii::Image			image = nullptr;
	vk::raii::DeviceMemory	memory = nullptr;
};

struct BufferData
{
	vk::raii::Buffer		buffer = nullptr;
	vk::raii::DeviceMemory	memory = nullptr;
};

struct UniformBufferData
{
	vk::DeviceSize			size;
	std::vector<BufferData>	bufferData;
	std::vector<void *>		mapped;
};

struct PipelineData
{
	PipelineInfo *							pipelineInfo;
	vk::raii::Pipeline 						pipeline = nullptr;
	vk::raii::PipelineLayout 				layout = nullptr;
	vk::raii::DescriptorSetLayout			descriptorSetLayout = nullptr;
	std::vector<vk::raii::DescriptorSet>	descriptorSets;
	UniformBufferData						uniforms;
	TextureData								textures;
};

struct VKValueConverter
{
	static constexpr vk::Format getType(AttributeType type)
	{
		switch (static_cast<int>(type))
		{
			case FLOAT3: return vk::Format::eR32G32B32Sfloat;
			case FLOAT2: return vk::Format::eR32G32Sfloat;
		}
		return vk::Format::eUndefined;
	}

	static constexpr vk::DescriptorType getDescriptorType(DescriptorType type)
	{
		switch (static_cast<int>(type))
		{
			case UNIFORM_BUFFER: return vk::DescriptorType::eUniformBuffer;
			case COMBINED_IMAGE_SAMPLER: return vk::DescriptorType::eCombinedImageSampler;
		}
		return vk::DescriptorType::eSampler;
	}

	static constexpr vk::ShaderStageFlagBits getShaderStage(ShaderStage stage)
	{
		switch (static_cast<int>(stage))
		{
			case VERTEX: return vk::ShaderStageFlagBits::eVertex;
			case FRAGMENT: return vk::ShaderStageFlagBits::eFragment;
		}
		return vk::ShaderStageFlagBits::eAll;
	}
};

struct PendingAsset
{
	Asset *			asset;
	PipelineID		pipelineID;
	BufferData		vertexData;
	BufferData		stagingVertexData;
	BufferData		indexData;
	BufferData		stagingIndexData;
};

class VulkanEngine : public AEngine
{
	public:
		VulkanEngine(GLFWwindow * window, Camera * camera);
		~VulkanEngine();

		void								load() override;
		void								beginFrame() override;
		AssetID								uploadAsset(Asset & asset, PipelineID pipelineID) override;
		void								unloadAsset(AssetID assetID) override;
		PipelineID							uploadPipeline(PipelineInfo & pipelineInfo) override;
		void								drawAsset(AssetID assetID, PipelineID pipelineID) override;
		void								endFrame() override;

	private:
		typedef std::vector<char const *>													RequiredExtensions;
		typedef std::vector<char const *>													RequiredLayers;
		typedef std::vector<vk::raii::CommandBuffer>										CommandBuffers;
		typedef std::vector<vk::raii::Semaphore>											Semaphores;
		typedef std::vector<vk::raii::Fence>												Fences;
		typedef std::vector<vk::VertexInputAttributeDescription>							VertexAttributeDescriptionVector;
		typedef std::unordered_map<PipelineID, std::vector<Asset *>>						PipelineAssetMap;
		typedef std::unordered_map<PipelineID, PipelineData>								PipelineMap;
		typedef std::unordered_map<std::string, std::shared_ptr<vk::raii::ShaderModule>>	ShaderCache;
		typedef std::unordered_map<AssetID, BufferData>										BufferCache;

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
	
		// Commands & Descriptor
		vk::raii::DescriptorPool			_descriptorPool = nullptr;
		vk::raii::CommandPool				_commandPool = nullptr;
		CommandBuffers						_commandBuffers;

		// Sync primitives
		Semaphores							_presentCompleteSemaphores;
		Semaphores							_renderFinishedSemaphores;
		Fences								_inFlightFences;
		uint32_t							_presentSemaphoreIndex = 0;
		uint32_t							_currentFrame = 0;
		uint32_t							_imageIndex = 0;

		// Buffers & Memory
		ShaderCache							_shaderCache;
		BufferCache							_vboCache;
		BufferCache							_iboCache;
		unsigned int						_nextAssetID = 0;

		// Depth test
		vk::raii::Image						_depthImage = nullptr;
		vk::raii::DeviceMemory				_depthImageMemory = nullptr;
		vk::raii::ImageView					_depthImageView = nullptr;
		vk::Format							_depthFormat;
		vk::ImageAspectFlags				_depthFlags;

		// Maps
		PipelineMap							_pipelineMap;
		PipelineAssetMap					_pipelineAssetMap;
		std::vector<PendingAsset>			_pendingAssets;

		void								_createInstance();
		void								_initDebugMessenger();
		void								_createSurface();
		RequiredExtensions					_getRequiredExtensions() const;
		RequiredLayers						_getRequiredLayers() const;
		void								_selectPhysicalDevice();
		QueueIndices						_findQueueFamilies() const;
		void								_createLogicalDevice();
		void								_checkDeviceExtensions(const std::vector<const char *> & deviceExtensions) const;
		vk::SurfaceFormatKHR				_chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> & formats);
		vk::PresentModeKHR					_chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> & presentModes);
		vk::Extent2D						_chooseSwapExtent(const vk::SurfaceCapabilitiesKHR & capabilities);
		void								_createSwapChain();
		void								_createImageViews();
		void								_createGraphicsPipelines();
		vk::raii::ShaderModule				_createShaderModule(const std::vector<char> & shaderSrc) const;
		void								_createCommandPool(vk::CommandPoolCreateFlags flags);
		void								_concateneVertexBuffer(Asset & asset);
		void								_concateneIndexBuffer(Asset & asset);
		void								_createVertexBuffer(PendingAsset & pendingAsset);
		void								_createIndexBuffer(PendingAsset & pendingAsset);
		void								_uploadPendingAssets();
		void								_createCommandBuffer();
		void								_recordCommandBuffer();
		void								_transitionImageViewLayout(TransitionImageViewLayoutInfo info);
		void								_transitionImageLayout(const vk::raii::Image & image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
		void								_transitionDepthImage();
		void								_createSyncObjects();
		void								_recreateSwapchain();
		uint32_t							_findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
		void 								_createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer & buffer, vk::raii::DeviceMemory & deviceMemory);
		void								_copyBuffer(vk::raii::Buffer & srcBuffer, vk::raii::Buffer & dstBuffer, vk::DeviceSize size);
		vk::raii::CommandBuffer				_beginSingleTimeCommands();
		void								_endSingleTimeCommands(vk::raii::CommandBuffer & commandBuffer);
		vk::VertexInputBindingDescription	_getBindingDescription(PipelineInfo & pipelineInfo) const;
		VertexAttributeDescriptionVector	_getAttributeDescription(PipelineInfo & pipelineInfo) const;
		void								_createDescriptorPool();
		void								_createDescriptorSetLayout(PipelineData & pipelineData);
		void								_createDescriptorSets(PipelineData & pipelineData);
		void								_createUniformBuffers(PipelineData & pipelineData);
		void								_updateUniformBuffer();
		void								_createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image & image, vk::raii::DeviceMemory & imageMemory, vk::SampleCountFlagBits sampling);
		void								_createTextureImage(PipelineData & pipelineData);
		void								_createTextureImageView(PipelineData & pipelineData);
		void								_createTextureSampler(PipelineData & pipelineData);
		void								_copyBufferToImage(const vk::raii::Buffer & buffer, vk::raii::Image & image, uint32_t width, uint32_t height);
		void								_createDepthResources();
		vk::Format							_findSupportedFormat(const std::vector<vk::Format> & candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
		vk::Format							_findDepthFormat();
};
