#pragma once

#include "vulkan_types.h"

struct VulkanSwapchainSupport
{
	VkSurfaceCapabilitiesKHR Capabilities;
	u32 FormatCount;
	VkSurfaceFormatKHR *Formats;
	u32 PresentModeCount;
	VkPresentModeKHR *PresentModes;
};

b8 VulkanSwapchainQuerySupport(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface, MemArena *Arena,
							   VulkanSwapchainSupport &OutSwapchainInfo);

void VulkanSwapchainCreate(VulkanContext *Context, u32 Width, u32 Height, MemArena *RendererArena,
						   VulkanSwapchain *OutSwapchain);

void VulkanSwapchainDestroy(VulkanContext *Context, VulkanSwapchain *Swapchain);

void VulkanSwapchainRecreate(VulkanContext *Context, u32 Width, u32 Height, VulkanSwapchain *OutSwapchain);

b8 VulkanSwapchainAcquireNextImageIndex(VulkanContext *Context, VulkanSwapchain *Swapchain,
										u64 TimeoutNs, VkSemaphore ImageAvailableSemaphore,
										VkFence Fence, u32 *OutImageIndex);

void VulkanSwapchainPresent(VulkanContext *Context, VulkanSwapchain *Swapchain,
							VkSemaphore RenderCompleteSemaphore, u32 PresentImageIndex);