/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 15:27:14 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/24 19:17:58 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <sstream>
#define VK_USE_PLATFORM_XCB_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <optional>

struct QueueFamilyIndices;
struct SwapChainSupportDetails;

/* ************************************************************************** */
/*   general_tools                                                            */
/* ************************************************************************** */

/* * Converts a value to a string.
 * This is a utility function that can be used to convert any type that supports
 * the stream insertion operator (<<) to a string.
 */
template <typename T>
const std::string toString(const T & value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

/* ************************************************************************** */
/*   vulkan_utils                                                             */
/* ************************************************************************** */

/* * Returns the required extensions for GLFW.
 * This function retrieves the required Vulkan extensions for GLFW and returns them
 * as a vector of C-style strings.
 */
std::vector<const char * > getRequiredExtensions();

# ifdef DEBUG
/* * Populates the debug messenger create info structure.
 * This function fills the VkDebugUtilsMessengerCreateInfoEXT structure with the
 * necessary information for debugging Vulkan applications.
 */
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo);
# endif

/* * Checks if a physical device is suitable for Vulkan.
 * This function checks if the given physical device supports the necessary features
 * and queue families to be used with Vulkan.
 */
bool isDeviceSuitable(const VkPhysicalDevice & device, const QueueFamilyIndices & indices, const VkSurfaceKHR & surface);

/* * Finds the queue families for a physical device.
 * This function checks the physical device for available queue families and returns
 * a QueueFamilyIndices structure containing the indices of the graphics family.
 */
QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice & device, const VkSurfaceKHR & surface);

SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice & device, const VkSurfaceKHR & surface);

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats);

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes);

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities, GLFWwindow * window);
