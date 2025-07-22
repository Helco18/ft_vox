/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 15:27:14 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/22 19:22:12 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <sstream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <optional>

struct QueueFamilyIndices;

template <typename T>
const std::string toString(const T & value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

std::vector<const char * > getRequiredExtensions();
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo);
bool isDeviceSuitable(const VkPhysicalDevice & device);
QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice & device);
