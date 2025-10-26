#pragma once

#include "vulkan_types.h"

void VulkanFenceCreate(VulkanContext *Context, b8 CreateSignaled, VulkanFence *OutFence);

void VulkanFenceDestroy(VulkanContext *Context, VulkanFence *Fence);

b8 VulkanFenceWait(VulkanContext *Context, VulkanFence *Fence, u64 TimeoutNS);

void VulkanFenceReset(VulkanContext *Context, VulkanFence *Fence);