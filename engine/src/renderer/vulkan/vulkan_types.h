#pragma once

#include "defines.h"
#include "containers/karray.h"
#include <vulkan/vulkan.h>

#define VK_CHECK(Expression) Assert((Expression) == VK_SUCCESS)

struct VulkanSwapchainSupport
{
	VkSurfaceCapabilitiesKHR Capabilities;
	u32 FormatCount;
	VkSurfaceFormatKHR *Formats;
	u32 PresentModeCount;
	VkPresentModeKHR *PresentModes;
};

struct VulkanDevice
{
	VkPhysicalDevice PhysicalDevice;
	VkDevice LogicalDevice;
	VulkanSwapchainSupport SwapchainSupport;

	u32 GraphicsIndex;
	u32 PresentIndex;
	u32 TransferIndex;
	// NOTE: We will probably not use compute at first
	u32 ComputeIndex;

	VkQueue GraphicsQueue;
	VkQueue PresentQueue;
	VkQueue TransferQueue;
	VkQueue ComputeQueue;

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