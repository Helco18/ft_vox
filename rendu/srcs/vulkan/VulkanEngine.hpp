/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanEngine.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 15:05:49 by scraeyme          #+#    #+#             */
/*   Updated: 2025/08/04 19:17:09 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "tools/utils.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "colors.hpp"
#include <iostream>
#include <vector>
#include <set>

const std::vector<const char * > deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	inline bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR		capabilities;
    std::vector<VkSurfaceFormatKHR>	formats;
    std::vector<VkPresentModeKHR>	presentModes;
};

class VulkanEngine
{
	public:

		VulkanEngine(GLFWwindow * window);
		~VulkanEngine();

		void				init();
		void				destroy();
		void				drawFrame();

		const VkDevice &	getDevice() const { return _device; }

	private:

		GLFWwindow *		_window;
		QueueFamilyIndices	_queueFamilyIndices;
		VkSurfaceKHR		_surface;
		VkInstance			_instance;
		VkPhysicalDevice	_physicalDevice;
		VkDevice			_device;
		VkQueue				_graphicsQueue;
		VkQueue				_presentQueue;
		VkSwapchainKHR		_swapChain;
		VkFormat			_swapChainImageFormat;
		VkExtent2D			_swapChainExtent;
		VkRenderPass		_renderPass;
		VkPipeline			_graphicsPipeline;
		VkPipelineLayout	_pipelineLayout;
		VkCommandPool		_commandPool;
		VkCommandBuffer		_commandBuffer;
		VkSemaphore			_imageAvailableSemaphore;
		VkSemaphore			_renderFinishedSemaphore;
		VkFence				_inFlightFence;
		typedef std::vector<VkImage> t_swapChainImgs;
		t_swapChainImgs		_swapChainImages;
		typedef std::vector<const char * > t_layers;
		t_layers			_validationLayers;
		typedef std::vector<VkImageView> t_swapChainImgsViews;
		t_swapChainImgsViews _swapChainImageViews;
		typedef std::vector<VkFramebuffer> t_frameBuffers;
		t_frameBuffers		_swapChainFramebuffers;

		void				createInstance();
		void				createSurface();
		void				pickGraphicsCard();
		void				createLogicalDevice();
		void				createSwapChain();
		void				createImageViews();
		void				createRenderPass();
		void				createGraphicsPipeline();
		void				createFramebuffers();
		void				createCommandPool();
		void				createCommandBuffer();
		void				createSyncObjects();

		void				recordCommandBuffer(uint32_t imageIndex);

};
