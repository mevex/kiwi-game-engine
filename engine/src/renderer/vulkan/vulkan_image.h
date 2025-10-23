#pragma once

#include "renderer/vulkan/vulkan_types.h"

// TODO: We define this here for now
#define MEMORY_TYPE_INDEX_INVALID -1
u32 FindMemoryTypeIndex(VulkanContext *Context, u32 TypeFilter, u32 PropertyFlags);

void VulkanImageCreate(VulkanContext *Context, VkImageType Type,
					   u32 Width, u32 Height, VkFormat Format,
					   VkImageTiling Tiling, VkImageUsageFlags Usage,
					   VkMemoryPropertyFlags MemoryFlags, b8 CreateView,
					   VkImageAspectFlags ViewAspectFlags, VulkanImage *OutImage);

void VulkanImageCreateView(VulkanContext *Context, VkFormat Format,
						   VulkanImage *Image, VkImageAspectFlags AspectFlags);

void VulkanImageDestroy(VulkanContext *Context, VulkanImage *Image);