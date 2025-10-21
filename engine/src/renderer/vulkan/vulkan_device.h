#pragma once

#include "vulkan_types.h"

b8 VulkanDeviceCreate(VulkanContext *Context, MemArena *RendererArena);

void VulkanDeviceDestroy(VulkanContext *Context, MemArena *RendererArena);

b8 SelectPhysicalDevice(VulkanContext *Context, MemArena *RendererArena);