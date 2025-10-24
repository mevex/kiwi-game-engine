#pragma once

#include "vulkan_types.h"

void VulkanRenderPassCreate(VulkanContext *Context, u32 x, u32 y, u32 w, u32 h,
							f32 r, f32 g, f32 b, f32 a, f32 Depth, u32 Stencil,
							VulkanRenderPass *OutRenderPass);

void VulkanRenderPassDestroy(VulkanContext *Context, VulkanRenderPass *RenderPass);

void VulkanRenderPassBegin(VulkanCommandBuffer *CommandBuffer, VulkanRenderPass *RenderPass,
						   VkFramebuffer FrameBuffer);

void VulkanRenderPassEnd(VulkanCommandBuffer *CommandBuffer);