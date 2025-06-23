#pragma once

#include "defines.h"
#include "containers/karray.h"
#include <vulkan/vulkan.h>

#define VK_CHECK(Expression) Assert((Expression) == VK_SUCCESS)

struct VulkanSwapchainSupport
{
	VkSurfaceCapabilitiesKHR Capabilities;
	u32 FormatCount;
	KArray<VkSurfaceFormatKHR> Formats;
	u32 PresentModeCount;
	KArray<VkPresentModeKHR> PresentModes;
};

struct VulkanDevice
{
	VkPhysicalDevice PhysicalDevice;
	VkDevice LogicalDevice;
	VulkanSwapchainSupport SwapchainSupport;

	u32 GraphicsIndex;
	u32 PresentIndex;
	u32 ComputeIndex;
	u32 TransferIndex;

	VkPhysicalDeviceProperties Properties;
	VkPhysicalDeviceFeatures Features;
	VkPhysicalDeviceMemoryProperties MemoryProperties;
};

struct VulkanContext
{
	VkInstance Instance;
	VkAllocationCallbacks *Allocator;
	VkSurfaceKHR Surface;

	VulkanDevice Device;

#ifdef KIWI_SLOW
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif
};

#ifdef KIWI_SLOW
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
												   VkDebugUtilsMessageTypeFlagsEXT MessageTypes,
												   const VkDebugUtilsMessengerCallbackDataEXT *CallbackData,
												   void *UserData);
#endif