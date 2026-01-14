#include "VulkanEngine.hpp"
#include "Logger.hpp"
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

	LogSeverity severityType = INFO;
	if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
   		severityType = INFO;
	if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
		severityType = INFO;
	if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
		severityType = WARNING;
	if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
		severityType = ERROR;

	std::string messageType = "";
	if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
   		messageType = "General - ";
	if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
		messageType = "Validation - ";
	if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
		messageType = "Performance - ";

	Logger::log(ENGINE_VULKAN, severityType, messageType + pCallbackData->pMessage + ".");
	return vk::False;
}

void VulkanEngine::_initDebugMessenger()
{
	if (!g_debug)
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
