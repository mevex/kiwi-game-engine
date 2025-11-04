#include "vulkan_types.h"
#include "vulkan_backend.h"
#include "core/logger.h"

// TODO: We define this here for now
#define MEMORY_TYPE_INDEX_INVALID -1

void VulkanImage::Create(VkImageType Type, u32 InWidth, u32 InHeight, VkFormat Format, VkImageTiling Tiling,
						 VkImageUsageFlags Usage, VkMemoryPropertyFlags MemoryFlags, b8 bCreateView,
						 VkImageAspectFlags ViewAspectFlags)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	Width = InWidth;
	Height = InHeight;

	VkImageCreateInfo ImageCreateInfo = {};
	ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ImageCreateInfo.imageType = Type;
	ImageCreateInfo.format = Format;
	ImageCreateInfo.extent.width = Width;
	ImageCreateInfo.extent.height = Height;
	ImageCreateInfo.extent.depth = 1;				 // TODO: support configurable depth
	ImageCreateInfo.mipLevels = 4;					 // TODO: support mips
	ImageCreateInfo.arrayLayers = 1;				 // TODO: support num of layers
	ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: support configurable sample count
	ImageCreateInfo.tiling = Tiling;
	ImageCreateInfo.usage = Usage;
	ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: support configurable sharing mode
	ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VK_CHECK(vkCreateImage(Context->Device.LogicalDevice, &ImageCreateInfo, Context->Allocator,
						   &Handle));

	VkMemoryRequirements MemoryRequirements;
	vkGetImageMemoryRequirements(Context->Device.LogicalDevice, Handle, &MemoryRequirements);

	i32 MemTypeIdx = FindMemoryTypeIndex(MemoryRequirements.memoryTypeBits, MemoryFlags);
	if (MemTypeIdx == MEMORY_TYPE_INDEX_INVALID)
	{
		LogError("Could not get the required memory type. Image not valid.");
	}

	// Allocate memory
	VkMemoryAllocateInfo MemAllocInfo = {};
	MemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemAllocInfo.allocationSize = MemoryRequirements.size;
	MemAllocInfo.memoryTypeIndex = MemTypeIdx;
	VK_CHECK(vkAllocateMemory(Context->Device.LogicalDevice, &MemAllocInfo,
							  Context->Allocator, &Memory));

	// Bind memory
	// TODO: Support configurable memory offset for image pools
	VK_CHECK(vkBindImageMemory(Context->Device.LogicalDevice, Handle, Memory, 0));

	if (bCreateView)
	{
		CreateView(Format, ViewAspectFlags);
	}
}

void VulkanImage::Destroy()
{
	VulkanContext *Context = &VulkanRenderer::Context;

	if (View)
	{
		vkDestroyImageView(Context->Device.LogicalDevice, View, Context->Allocator);
		View = 0;
	}

	if (Memory)
	{
		vkFreeMemory(Context->Device.LogicalDevice, Memory, Context->Allocator);
		Memory = 0;
	}

	if (Handle)
	{
		vkDestroyImage(Context->Device.LogicalDevice, Handle, Context->Allocator);
		Handle = 0;
	}
}

void VulkanImage::CreateView(VkFormat Format, VkImageAspectFlags AspectFlags)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	VkImageViewCreateInfo ViewCreateInfo = {};
	ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ViewCreateInfo.image = Handle;
	ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ViewCreateInfo.format = Format;
	ViewCreateInfo.subresourceRange.aspectMask = AspectFlags;
	// TODO: Support configuration of the followings
	ViewCreateInfo.subresourceRange.baseMipLevel = 0;
	ViewCreateInfo.subresourceRange.levelCount = 1;
	ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	ViewCreateInfo.subresourceRange.layerCount = 1;

	VK_CHECK(vkCreateImageView(Context->Device.LogicalDevice, &ViewCreateInfo, Context->Allocator, &View));
}

u32 VulkanImage::FindMemoryTypeIndex(u32 TypeFilter, u32 PropertyFlags)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	VkPhysicalDeviceMemoryProperties MemProperties;
	vkGetPhysicalDeviceMemoryProperties(Context->Device.PhysicalDevice, &MemProperties);

	for (u32 Idx = 0; Idx < MemProperties.memoryTypeCount; ++Idx)
	{
		if (TypeFilter & (1 << Idx) &&
			CheckFlags(MemProperties.memoryTypes[Idx].propertyFlags, PropertyFlags))
		{
			return Idx;
		}
	}

	LogWarning("Unable to find a suitable memory type");
	return (u32)MEMORY_TYPE_INDEX_INVALID;
}