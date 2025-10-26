#pragma once

#include "vulkan_types.h"

void VulkanFramebufferCreate(VulkanContext *Context, VulkanRenderPass *RenderPass,
							 u32 Width, u32 Height, u32 AttachmentCount, VkImageView *Attachments,
							 VulkanFramebuffer *OutFramebuffer);

void VulkanFramebufferDestroy(VulkanContext *Context, VulkanFramebuffer *Framebuffer);