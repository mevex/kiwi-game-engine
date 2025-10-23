#include "renderer/vulkan/vulkan_image.h"
#include "renderer/vulkan/vulkan_device.h"
#include "core/kiwi_mem.h"
#include "core/logger.h"

u32 FindMemoryTypeIndex(VulkanContext *Context, u32 TypeFilter, u32 PropertyFlags)
{
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

void VulkanImageCreate(VulkanContext *Context, VkImageType Type,
					   u32 Width, u32 Height, VkFormat Format,
					   VkImageTiling Tiling, VkImageUsageFlags Usage,
					   VkMemoryPropertyFlags MemoryFlags, b8 CreateView,
					   VkImageAspectFlags ViewAspectFlags, VulkanImage *OutImage)
{
	OutImage->Width = Width;
	OutImage->Height = Height;

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
						   &OutImage->Handle));

	VkMemoryRequirements MemoryRequirements;
	vkGetImageMemoryRequirements(Context->Device.LogicalDevice, OutImage->Handle, &MemoryRequirements);

	i32 MemTypeIdx = FindMemoryTypeIndex(Context, MemoryRequirements.memoryTypeBits, MemoryFlags);
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
							  Context->Allocator, &OutImage->Memory));

	// Bind memory
	// TODO: Support configurable memory offset for image pools
	VK_CHECK(vkBindImageMemory(Context->Device.LogicalDevice, OutImage->Handle, OutImage->Memory, 0));

	if (CreateView)
	{
		VulkanImageCreateView(Context, Format, OutImage, ViewAspectFlags);
	}
}

void VulkanImageCreateView(VulkanContext *Context, VkFormat Format,
						   VulkanImage *Image, VkImageAspectFlags AspectFlags)
{
	VkImageViewCreateInfo ViewCreateInfo = {};
	ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ViewCreateInfo.image = Image->Handle;
	ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ViewCreateInfo.format = Format;
	ViewCreateInfo.subresourceRange.aspectMask = AspectFlags;
	// TODO: Support configuration of the followings
	ViewCreateInfo.subresourceRange.baseMipLevel = 0;
	ViewCreateInfo.subresourceRange.levelCount = 1;
	ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	ViewCreateInfo.subresourceRange.layerCount = 1;

	VK_CHECK(vkCreateImageView(Context->Device.LogicalDevice, &ViewCreateInfo, Context->Allocator,
							   &Image->View));
}

void VulkanImageDestroy(VulkanContext *Context, VulkanImage *Image)
{
	if (Image->View)
	{
		vkDestroyImageView(Context->Device.LogicalDevice, Image->View, Context->Allocator);
		Image->View = 0;
	}

	if (Image->Memory)
	{
		vkFreeMemory(Context->Device.LogicalDevice, Image->Memory, Context->Allocator);
		Image->Memory = 0;
	}

	if (Image->Handle)
	{
		vkDestroyImage(Context->Device.LogicalDevice, Image->Handle, Context->Allocator);
		Image->Handle = 0;
	}
}