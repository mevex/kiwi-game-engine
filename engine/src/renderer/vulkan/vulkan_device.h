#pragma once

#include "vulkan_types.h"

b8 VulkanDeviceCreate(VulkanContext *Context);

void VulkanDeviceDestroy(VulkanContext *Context);

b8 SelectPhysicalDevice(VulkanContext *Context);

b8 VulkanDeviceDetectDepthFormat(VulkanDevice *Device);