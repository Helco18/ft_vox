/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanEngine.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 15:05:49 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/25 19:34:30 by scraeyme         ###   ########.fr       */
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
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanEngine
{
	public:

		VulkanEngine(GLFWwindow * window);
		~VulkanEngine();

		void init();
		void destroy();

	private:

		GLFWwindow *		_window;
		QueueFamilyIndices	_indices;
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

		typedef std::vector<VkImage> t_swapChainImgs;
		t_swapChainImgs _swapChainImages;
		typedef std::vector<const char * > t_layers;
		t_layers _validationLayers;
		typedef std::vector<VkImageView> t_swapChainImgsViews;
		t_swapChainImgsViews _swapChainImageViews;
		typedef std::vector<VkFramebuffer> t_frameBuffers;
		t_frameBuffers _swapChainFramebuffers;

		void createInstance();
		void createSurface();
		void pickGraphicsCard();
		void createLogicalDevice();
		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createGraphicsPipeline();
		void createFramebuffers();

};
