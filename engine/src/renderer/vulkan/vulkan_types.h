#pragma once

#include "defines.h"
#include <vulkan/vulkan.h>

#define VK_CHECK(Expression) Assert((Expression) == VK_SUCCESS)

struct VulkanContext
{
	VkInstance Instance;
	VkAllocationCallbacks *Allocator;

#if defined(KIWI_SLOW)
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif
};

#if defined(KIWI_SLOW)
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
												   VkDebugUtilsMessageTypeFlagsEXT MessageTypes,
												   const VkDebugUtilsMessengerCallbackDataEXT *CallbackData,
												   void *UserData);
#endif