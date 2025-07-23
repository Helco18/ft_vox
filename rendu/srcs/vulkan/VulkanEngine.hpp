/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanEngine.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcannaud <gcannaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 15:05:49 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/23 17:29:00 by gcannaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifdef DEBUG
# define IS_DEBUG 1
# else
# define IS_DEBUG 0
#endif

#include "tools/utils.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "colors.hpp"
#include <iostream>
#include <vector>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
};

class VulkanEngine
{
	public:

		VulkanEngine(GLFWwindow *window);
		~VulkanEngine();

		void init();
		void destroy();

	private:

		GLFWwindow * _window;
		VkSurfaceKHR _surface;
		VkInstance _instance;
		VkPhysicalDevice _physicalDevice;
		VkDevice _device;
		VkQueue _graphicsQueue;
		QueueFamilyIndices _indices;

		typedef std::vector<const char * > t_layers;
		t_layers _validationLayers;

		void createInstance();
		void pickGraphicsCard();
		void createLogicalDevice();
		void createSurface();

};
