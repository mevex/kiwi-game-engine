#pragma once

#include "vulkan_types.h"

struct PhysicalDeviceQueueCounts
{
	u32 GraphicsQueueCount;
	u32 PresentQueueCount;
	u32 TransferQueueCount;
	u32 ComputeQueueCount;
};

b8 VulkanDeviceCreate(VulkanContext *Context);

void VulkanDeviceDestroy(VulkanContext *Context);

b8 SelectPhysicalDevice(VulkanContext *Context, PhysicalDeviceQueueCounts &OutQueueCounts);

b8 VulkanDeviceDetectDepthFormat(VulkanDevice *Device);