#include "VulkanEngine.hpp"
#include "Gui.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

void VulkanEngine::_initImGui()
{
	std::vector<vk::DescriptorPoolSize> poolSizes =
	{
		{ vk::DescriptorType::eSampler, 1000 },
		{ vk::DescriptorType::eCombinedImageSampler, 1000 },
		{ vk::DescriptorType::eSampledImage, 1000 },
		{ vk::DescriptorType::eStorageImage, 1000 },
		{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
		{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
		{ vk::DescriptorType::eUniformBuffer, 1000 },
		{ vk::DescriptorType::eStorageBuffer, 1000 },
		{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
		{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
		{ vk::DescriptorType::eInputAttachment, 1000 }
	};

	vk::DescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	descriptorPoolInfo.maxSets = 11000 * MAX_FRAMES_IN_FLIGHT;
	descriptorPoolInfo.poolSizeCount = poolSizes.size();
	descriptorPoolInfo.pPoolSizes = poolSizes.data();

	_imGuiPool = vk::raii::DescriptorPool( _device, descriptorPoolInfo );

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	vk::PipelineRenderingCreateInfo renderingInfo{};
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachmentFormats = &_swapChainImageFormat;
	// optional but recommended:
	renderingInfo.depthAttachmentFormat = vk::Format::eD24UnormS8Uint;
	renderingInfo.stencilAttachmentFormat = vk::Format::eD24UnormS8Uint;

	ImGui_ImplGlfw_InitForVulkan(_window, true);
	ImGui_ImplVulkan_InitInfo initInfo = {};
	initInfo.Instance = *_instance;
	initInfo.PhysicalDevice = *_physicalDevice;
	initInfo.Device = *_device;
	initInfo.QueueFamily = _queueIndices.graphicsIndex;
	initInfo.Queue = *_queue;
	initInfo.DescriptorPool = *_imGuiPool;
	initInfo.MinImageCount = 2;
	initInfo.ImageCount = _swapChainImages.size();
	initInfo.PipelineInfoMain.PipelineRenderingCreateInfo = renderingInfo;
	initInfo.UseDynamicRendering = true;
	ImGui_ImplVulkan_Init(&initInfo);

	ImGuiIO &io = ImGui::GetIO();
	ImFontConfig config;
	config.OversampleH = 3;
	config.OversampleV = 3;
	config.GlyphExtraAdvanceX = 0.8f;
	io.Fonts->AddFontFromFileTTF("resources/assets/font/Minecraft.ttf", 14.0f, &config);
	io.Fonts->Build();

	ImGuiStyle & style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.5f, 0.0f, 0.9f, 0.002f);
}

void VulkanEngine::beginImGui()
{
	if (!_imGuiEnabled)
		return;
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	Gui::generateGui(_window);
	_imGuiThisFrame = true;
}

void VulkanEngine::_renderImGui(vk::CommandBuffer & commands)
{
	if (!_imGuiThisFrame)
		return;
	ImGui::Render();
	ImGui::GetDrawData();

	vk::RenderingAttachmentInfo colorAttachmentInfo;
	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	colorAttachmentInfo.imageView = _swapChainImageViews[_imageIndex];
	colorAttachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
	colorAttachmentInfo.loadOp = vk::AttachmentLoadOp::eLoad;
	colorAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachmentInfo.clearValue = clearColor;

	vk::RenderingAttachmentInfo depthAttachmentInfo;
	vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0.0f);
	depthAttachmentInfo.imageView = _depthImageView;
	depthAttachmentInfo.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	depthAttachmentInfo.loadOp = vk::AttachmentLoadOp::eLoad;
	depthAttachmentInfo.storeOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachmentInfo.clearValue = clearDepth;

	vk::RenderingInfo renderingInfo;
	renderingInfo.renderArea.offset.x = 0;
	renderingInfo.renderArea.offset.y = 0;
	renderingInfo.renderArea.extent = _swapChainExtent;
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachmentInfo;
	renderingInfo.pDepthAttachment = &depthAttachmentInfo;
	vk::CommandBufferBeginInfo beginInfo;
	vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
	beginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commands);
	_imGuiThisFrame = false;
}

void VulkanEngine::_shutdownImGui()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplVulkan_Shutdown();
	ImGui::DestroyContext();
}