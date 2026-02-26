#include "VulkanEngine.hpp"
#include "Gui.hpp"
#include "Logger.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"
#include "utils.hpp"

void VulkanEngine::_initImGui()
{
	const std::vector<vk::DescriptorPoolSize> poolSizes =
	{
		{ vk::DescriptorType::eSampler, 100 },
		{ vk::DescriptorType::eCombinedImageSampler, 100 },
		{ vk::DescriptorType::eUniformBuffer, 100 },
	};

	vk::DescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	descriptorPoolInfo.maxSets = 300 * MAX_FRAMES_IN_FLIGHT;
	descriptorPoolInfo.poolSizeCount = poolSizes.size();
	descriptorPoolInfo.pPoolSizes = poolSizes.data();

	_imGuiPool = vk::raii::DescriptorPool( _device, descriptorPoolInfo );
	_imGuiCommandBuffers = _createCommandBuffers(_graphicsCommandPool, vk::CommandBufferLevel::eSecondary, MAX_FRAMES_IN_FLIGHT);

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
	initInfo.Queue = *_graphicsQueue;
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

void VulkanEngine::_renderImGui()
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

	vk::RenderingInfo renderingInfo;
	renderingInfo.renderArea.offset.x = 0;
	renderingInfo.renderArea.offset.y = 0;
	renderingInfo.renderArea.extent = _swapChainExtent;
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachmentInfo;
	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	vk::CommandBuffer commands = _imGuiCommandBuffers[_currentFrame];
	commands.begin(commandBufferBeginInfo);
	commands.beginRendering(renderingInfo);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commands);
	commands.endRendering();
	commands.end();
	_graphicsCommandBuffers[_currentFrame].executeCommands(commands);
	_imGuiThisFrame = false;
}

void VulkanEngine::_shutdownImGui()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplVulkan_Shutdown();
	ImGui::DestroyContext();
}