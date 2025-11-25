#include "VulkanEngine.hpp"
#include "colors.hpp"
#include <iostream>

static VULKAN_CALLBACK debugCallback(
	vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
	vk::DebugUtilsMessageTypeFlagsEXT type,
	const vk::DebugUtilsMessengerCallbackDataEXT * pCallbackData,
	void *
)
{
	if (severity < DEBUG_LEVEL)
		return vk::False;

	std::string severityType = "";
	if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
   		severityType = GRAY "[VERBOSE] ";
	if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
		severityType = CYAN "[INFO] ";
	if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
		severityType = YELLOW "[WARNING] ";
	if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
		severityType = RED "[ERROR] ";

	std::string messageType = "";
	if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
   		messageType = "General - ";
	if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
		messageType = "Validation - ";
	if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
		messageType = "Performance - ";

	std::cout << severityType << messageType << pCallbackData->pMessage << RESET << std::endl;
	return vk::False;
}

void VulkanEngine::_initDebugMessenger()
{
	if (!g_enableValidationLayers)
		return;

	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
														| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
														| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
														| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
														| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
														| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
	vk::DebugUtilsMessengerCreateInfoEXT debugStruct;
	debugStruct.messageSeverity = severityFlags;
	debugStruct.messageType = messageTypeFlags;
	debugStruct.pfnUserCallback = &debugCallback;
	_debugMessenger = _instance.createDebugUtilsMessengerEXT(debugStruct);
}
