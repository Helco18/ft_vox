/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanEngine.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 15:05:49 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/24 18:43:09 by scraeyme         ###   ########.fr       */
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

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	inline bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
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
		VkQueue _presentQueue;
		QueueFamilyIndices _indices;

		typedef std::vector<const char * > t_layers;
		t_layers _validationLayers;

		void createInstance();
		void pickGraphicsCard();
		void createLogicalDevice();
		void createSurface();

};
