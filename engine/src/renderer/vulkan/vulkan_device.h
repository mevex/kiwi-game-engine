#pragma once

#include "vulkan_types.h"

b8 VulkanDeviceCreate(VulkanContext *Context);

void VulkanDeviceDestroy(VulkanContext *Context);

void VulkanDeviceQuerySwapchainSupport(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface,
									   VulkanSwapchainSupport &OutSwapchainInfo);