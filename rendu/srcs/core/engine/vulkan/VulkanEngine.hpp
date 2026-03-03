#pragma once

#define VULKAN_CALLBACK VKAPI_ATTR vk::Bool32 VKAPI_CALL
#define DEBUG_LEVEL vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
#define MAX_FRAMES_IN_FLIGHT 2
#define VULKAN_SHADER_PATH "resources/shaders/spir-v/"

#define VK_USE_PLATFORM_XCB_KHR
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include "AEngine.hpp"
#include "ThreadPool.hpp"
#include <vulkan/vulkan_raii.hpp>

struct VKValueConverter
{
	static constexpr vk::Format getType(AttributeType type)
	{
		switch (type)
		{
			case FLOAT3: return vk::Format::eR32G32B32Sfloat;
			case FLOAT2: return vk::Format::eR32G32Sfloat;
			case FLOAT: return vk::Format::eR32Sfloat;
			case INT: return vk::Format::eR16Sint;
		}
		return vk::Format::eUndefined;
	}

	static constexpr vk::DescriptorType getDescriptorType(DescriptorType type)
	{
		switch (type)
		{
			case UNIFORM_BUFFER: case PUSH_CONSTANT: return vk::DescriptorType::eUniformBuffer;
			case COMBINED_IMAGE_SAMPLER: return vk::DescriptorType::eCombinedImageSampler;
		}
		return vk::DescriptorType::eSampler;
	}

	static constexpr vk::PrimitiveTopology getDrawMode(DrawMode drawMode)
	{
		switch (drawMode)
		{
			case DrawMode::TRIANGLES: return vk::PrimitiveTopology::eTriangleList;
			case DrawMode::LINES: return vk::PrimitiveTopology::eLineList;
		}
		return vk::PrimitiveTopology::eTriangleList;
	}

	static constexpr vk::ShaderStageFlagBits getShaderStage(ShaderStage stage)
	{
		switch (stage)
		{
			case VERTEX: return vk::ShaderStageFlagBits::eVertex;
			case FRAGMENT: return vk::ShaderStageFlagBits::eFragment;
			case ALL: return vk::ShaderStageFlagBits::eAll;
		}
		return vk::ShaderStageFlagBits::eAll;
	}

	static constexpr vk::Filter getFilter(TextureFiltering textureFiltering)
	{
		switch (textureFiltering)
		{
			case LINEAR: return vk::Filter::eLinear;
			case NEAREST: return vk::Filter::eNearest;
		}
		return vk::Filter::eLinear;
	}

	static constexpr vk::SamplerMipmapMode getSamplerFilter(TextureFiltering textureFiltering)
	{
		switch (textureFiltering)
		{
			case LINEAR: return vk::SamplerMipmapMode::eLinear;
			case NEAREST: return vk::SamplerMipmapMode::eNearest;
		}
		return vk::SamplerMipmapMode::eLinear;
	}
};

struct QueueIndices
{
	uint32_t	graphicsIndex;
	uint32_t	presentIndex;
	uint32_t	transferIndex;
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
	unsigned int			textureID;
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
	PipelineInfo										pipelineInfo;
	vk::raii::Pipeline 									pipeline = nullptr;
	vk::raii::PipelineLayout 							layout = nullptr;
	std::vector<vk::raii::CommandBuffer> 				commandBuffers;
	vk::raii::DescriptorPool							descriptorPool = nullptr;
	vk::raii::DescriptorSetLayout						descriptorSetLayout = nullptr;
	std::vector<vk::raii::DescriptorSet>				descriptorSets;
	std::unordered_map<unsigned int, UniformBufferData>	uniforms;
	std::unordered_map<unsigned int, TextureData>		textures;
};

struct PendingUniform
{
	PipelineID		pipelineID;
	unsigned int	binding;
	void *			data;
	size_t			size;
};

struct PendingAsset
{
	Asset *			asset;
	PipelineID		pipelineID;
	BufferData		vertexData;
	BufferData		stagingVertexData;
	vk::DeviceSize	vertexRingBufferOffset;
	BufferData		indexData;
	BufferData		stagingIndexData;
	vk::DeviceSize	indexRingBufferOffset;
};

struct AssetData
{
	Asset *									asset = nullptr;
	PipelineID								pipelineID;
	BufferData								vbo;
	BufferData								ibo;
};

class VulkanEngine : public AEngine
{
	public:
		VulkanEngine(GLFWwindow * window);
		~VulkanEngine();

		void								load() override;
		void								setVsync(bool vsync) override;
		void								beginFrame() override;
		AssetID								uploadAsset(Asset & asset, PipelineID pipelineID) override;
		void								unloadAsset(AssetID assetID) override;
		PipelineID							uploadPipeline(PipelineInfo & pipelineInfo) override;
		void								updateUniformBuffer(PipelineID pipelineID, unsigned int binding, void * data, size_t size) override;
		void								drawAsset(AssetID assetID, PipelineID pipelineID) override;
		void								endFrame() override;

		void								beginImGui() override;

	private:
		typedef std::vector<char const *>													RequiredExtensions;
		typedef std::vector<char const *>													RequiredLayers;
		typedef std::vector<vk::raii::CommandBuffer>										CommandBuffers;
		typedef std::vector<vk::raii::Semaphore>											Semaphores;
		typedef std::vector<vk::raii::Fence>												Fences;
		typedef std::vector<vk::VertexInputAttributeDescription>							VertexAttributeDescriptionVector;
		typedef std::unordered_map<std::string, std::shared_ptr<vk::raii::ShaderModule>>	ShaderCache;
		// Query with PipelineID
		typedef std::vector<std::vector<Asset *>>											DrawableAssets;
		// Query with PipelineID
		typedef std::vector<PipelineData>													PipelineCache;
		// Query with AssetID
		typedef std::unordered_map<AssetID, AssetData>										AssetDataCache;

		// Window, context, instance
		vk::raii::Context					_context;
		vk::raii::Instance					_instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT	_debugMessenger = nullptr;
		vk::raii::SurfaceKHR				_surface = nullptr;

		// Physical/Logical device
		vk::raii::PhysicalDevice			_physicalDevice = nullptr;
		vk::raii::Device					_device = nullptr;
		vk::raii::Queue						_graphicsQueue = nullptr;
		vk::raii::Queue						_transferQueue = nullptr;
		QueueIndices						_queueIndices;

		// Swapchain & Images
		vk::raii::SwapchainKHR				_swapChain = nullptr;
		vk::SurfaceFormatKHR				_swapChainSurfaceFormat;
		vk::Extent2D						_swapChainExtent;
		vk::Format							_swapChainImageFormat;
		std::vector<vk::Image>				_swapChainImages;
		std::vector<vk::raii::ImageView>	_swapChainImageViews;
		bool								_vsync = false;

		// MSAA
		vk::raii::Image						_msaaImage = nullptr;
		vk::raii::DeviceMemory				_msaaImageMemory = nullptr;
		vk::raii::ImageView					_msaaImageView = nullptr;

		// Commands & Descriptor
		vk::raii::CommandPool				_graphicsCommandPool = nullptr;
		vk::raii::CommandPool				_transferCommandPool = nullptr;
		CommandBuffers						_graphicsCommandBuffers;
		vk::raii::CommandBuffer				_transferCommandBuffer = nullptr;

		// Sync primitives
		Semaphores							_presentCompleteSemaphores;
		Semaphores							_renderFinishedSemaphores;
		Fences								_inFlightFences;
		uint32_t							_currentFrame = 0;
		uint32_t							_imageIndex = 0;

		// Buffers & Memory
		ShaderCache							_shaderCache;
		AssetDataCache						_assetDataCache;
		unsigned int						_nextAssetID = 0;
		unsigned int						_nextTextureID = 0;

		// Depth test
		vk::raii::Image						_depthImage = nullptr;
		vk::raii::DeviceMemory				_depthImageMemory = nullptr;
		vk::raii::ImageView					_depthImageView = nullptr;
		vk::Format							_depthFormat;
		vk::ImageAspectFlags				_depthFlags;

		// Caches
		PipelineCache						_pipelineCache;
		DrawableAssets						_drawableAssets;
		std::vector<PendingUniform>			_pendingUniforms;
		std::vector<PendingAsset>			_pendingAssets;
		BufferData							_stagingBuffer;
		vk::DeviceSize						_currentStagingBufferOffset = 0;
		vk::DeviceSize						_stagingBufferSize = 0;
		std::vector<std::vector<AssetID>>	_pendingUnloads;

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
		void								_createMultisamplingImage();
		void								_createGraphicsPipelines();
		vk::raii::ShaderModule				_createShaderModule(const std::vector<char> & shaderSrc) const;
		vk::raii::CommandPool				_createCommandPool(uint32_t queueIndex, vk::CommandPoolCreateFlags flags);
		void								_createVertexBuffer(PendingAsset & pendingAsset);
		void								_createIndexBuffer(PendingAsset & pendingAsset);
		void								_processPendingAssets();
		void								_processPendingUniforms();
		void								_processPendingUnloads();
		CommandBuffers						_createCommandBuffers(vk::raii::CommandPool & commandPool, vk::CommandBufferLevel level, uint8_t count);
		void								_recordCommandBuffer();
		void								_retrieveCommandBuffers();
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
		void								_createDescriptorPool(PipelineData & pipelineData);
		void								_createDescriptorSetLayout(PipelineData & pipelineData);
		void								_createDescriptorSets(PipelineData & pipelineData);
		void								_createUniformBuffers(PipelineData & pipelineData);
		void								_createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image & image, vk::raii::DeviceMemory & imageMemory, vk::SampleCountFlagBits sampling);
		void								_createTextures(PipelineData & pipelineData);
		void								_createTextureImage(TextureData & textureData, TextureInfo & textureInfo);
		void								_createTextureImageView(TextureData & textureData);
		void								_createTextureSampler(TextureData & textureData, TextureInfo & textureInfo);
		void								_copyBufferToImage(const vk::raii::Buffer & buffer, vk::raii::Image & image, uint32_t width, uint32_t height);
		void								_createDepthResources();
		vk::Format							_findSupportedFormat(const std::vector<vk::Format> & candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
		vk::Format							_findDepthFormat();

		// ImGui
		void								_initImGui();
		void								_renderImGui();
		void								_shutdownImGui();
		vk::raii::DescriptorPool			_imGuiPool = nullptr;
		CommandBuffers						_imGuiCommandBuffers;
};
