#pragma once

#include "vulkan_types.h"

void VulkanCommandBufferAllocate(VulkanContext *Context, VkCommandPool Pool, b8 IsPrimary,
								 VulkanCommandBuffer *OutCommandBuffer);

void VulkanCommandBufferFree(VulkanContext *Context, VkCommandPool Pool,
							 VulkanCommandBuffer *CommandBuffer);

void VulkanCommandBufferBegin(VulkanCommandBuffer *CommandBuffer, b8 IsSingleUse, b8 IsRenderPassContinue,
							  b8 IsSimultaneousUse);

void VulkanCommandBufferEnd(VulkanCommandBuffer *CommandBuffer);

void VulkanCommandBufferUpdateSubmitted(VulkanCommandBuffer *CommandBuffer);

void VulkanCommandBufferReset(VulkanCommandBuffer *CommandBuffer);

// Helper functions that allow to quickly get and use a single use command buffer
void VulkanCommandBufferAllocateAndBeginSingleUse(VulkanContext *Context, VkCommandPool Pool,
												  VulkanCommandBuffer *OutCommandBuffer);

void VulkanCommandBufferEndSingleUse(VulkanContext *Context, VkCommandPool Pool,
									 VulkanCommandBuffer *CommandBuffer, VkQueue Queue);