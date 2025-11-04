#include "vulkan_swapchain.h"

#include "core/logger.h"
#include "vulkan_device.h"
#include "vulkan_image.h"

b8 VulkanSwapchainQuerySupport(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface,
							   MemArena *Arena, VulkanSwapchainSupport &OutSwapchainSupport)
{
	// Surface capabilities
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface,
													   &OutSwapchainSupport.Capabilities));

	// Surface formats
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface,
												  &OutSwapchainSupport.FormatCount, nullptr));
	if (OutSwapchainSupport.FormatCount != 0)
	{
		OutSwapchainSupport.Formats =
			(VkSurfaceFormatKHR *)Arena->PushNoZero(OutSwapchainSupport.FormatCount * sizeof(VkSurfaceFormatKHR));
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &OutSwapchainSupport.FormatCount,
													  OutSwapchainSupport.Formats));
	}
	else
	{
		return false;
	}

	// Present modes
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface,
													   &OutSwapchainSupport.PresentModeCount, nullptr));
	if (OutSwapchainSupport.PresentModeCount != 0)
	{
		OutSwapchainSupport.PresentModes =
			(VkPresentModeKHR *)Arena->PushNoZero(OutSwapchainSupport.PresentModeCount * sizeof(VkPresentModeKHR));
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &OutSwapchainSupport.PresentModeCount,
														   OutSwapchainSupport.PresentModes));
	}
	else
	{
		Arena->Pop(OutSwapchainSupport.FormatCount * sizeof(VkSurfaceFormatKHR));
		return false;
	}

	return true;
}

void VulkanSwapchainCreate(VulkanContext *Context, u32 Width, u32 Height, MemArena *RendererArena,
						   VulkanSwapchain *OutSwapchain)
{
	AutoFreeArena ScratchArenaHandle(MemTag_Scratch);
	VulkanSwapchainSupport SwapchainSupport = {};
	if (!VulkanSwapchainQuerySupport(Context->Device.PhysicalDevice, Context->Surface,
									 ScratchArenaHandle.Arena, SwapchainSupport))
	{
		LogWarning("Could not query swapchain support during Swapchain creation");
		KDebugBreak();
	}

	OutSwapchain->MaxFramesInFlight = 2; // Triple buffering

	b8 Found = false;
	for (u32 Idx = 0; Idx < SwapchainSupport.FormatCount; ++Idx)
	{
		VkSurfaceFormatKHR Format = SwapchainSupport.Formats[Idx];
		// Prefered format type
		if (Format.format == VK_FORMAT_B8G8R8A8_UNORM &&
			Format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			OutSwapchain->ImageFormat = Format;
			Found = true;
			break;
		}
	}

	if (!Found)
		OutSwapchain->ImageFormat = SwapchainSupport.Formats[0];

	VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (u32 Idx = 0; Idx < SwapchainSupport.PresentModeCount; ++Idx)
	{
		VkPresentModeKHR Mode = SwapchainSupport.PresentModes[Idx];
		if (Mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			// NOTE: TODO: VSync activated by default for now
			PresentMode = Mode;
			break;
		}
	}

	VkExtent2D SwapchainExtent = {};
	if (SwapchainSupport.Capabilities.currentExtent.width != UINT32_MAX)
	{
		SwapchainExtent = SwapchainSupport.Capabilities.currentExtent;
	}

	VkExtent2D Min = SwapchainSupport.Capabilities.minImageExtent;
	VkExtent2D Max = SwapchainSupport.Capabilities.maxImageExtent;
	SwapchainExtent.width = Clamp(Width, Min.width, Max.width);
	SwapchainExtent.height = Clamp(Height, Min.height, Max.height);

	// NOTE: We expect the min to be 2, so the ImageCount should be set to 3. This is because we want
	// to use triple buffering and so while 1 image is being presented we render on the other 2.
	// However, we safeguard by checking the max to be sure that 3 is actually supported.
	// Not really necessary on today's hardware, but better safe than sorry.
	u32 ImageCount = SwapchainSupport.Capabilities.minImageCount + 1;
	if (SwapchainSupport.Capabilities.maxImageCount > 0 &&
		ImageCount > SwapchainSupport.Capabilities.maxImageCount)
	{
		ImageCount = SwapchainSupport.Capabilities.maxImageCount;
	}
	OutSwapchain->MaxFramesInFlight = (u8)ImageCount - 1;

	VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
	SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfo.surface = Context->Surface;
	SwapchainCreateInfo.minImageCount = ImageCount;
	SwapchainCreateInfo.imageFormat = OutSwapchain->ImageFormat.format;
	SwapchainCreateInfo.imageColorSpace = OutSwapchain->ImageFormat.colorSpace;
	SwapchainCreateInfo.imageExtent = SwapchainExtent;
	SwapchainCreateInfo.imageArrayLayers = 1;
	SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (Context->Device.GraphicsIndex != Context->Device.PresentIndex)
	{
		u32 *QueueFamilyIndices = (u32 *)ScratchArenaHandle.Arena->Push(sizeof(u32) * 2);
		QueueFamilyIndices[0] = Context->Device.GraphicsIndex;
		QueueFamilyIndices[1] = Context->Device.PresentIndex;

		SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		SwapchainCreateInfo.queueFamilyIndexCount = 2;
		SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
	}
	else
	{
		SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		SwapchainCreateInfo.queueFamilyIndexCount = 0;
		SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	SwapchainCreateInfo.preTransform = SwapchainSupport.Capabilities.currentTransform;
	SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapchainCreateInfo.presentMode = PresentMode;
	SwapchainCreateInfo.clipped = VK_TRUE;
	SwapchainCreateInfo.oldSwapchain = 0;

	VK_CHECK(vkCreateSwapchainKHR(Context->Device.LogicalDevice, &SwapchainCreateInfo, Context->Allocator,
								  &OutSwapchain->Handle));

	Context->CurrentFrame = 0;

	// Get Images
	VK_CHECK(vkGetSwapchainImagesKHR(Context->Device.LogicalDevice, OutSwapchain->Handle,
									 &OutSwapchain->ImageCount, 0));
	if (!OutSwapchain->Images)
	{
		OutSwapchain->Images = (VkImage *)RendererArena->Push(sizeof(VkImage) * OutSwapchain->ImageCount);
	}
	if (!OutSwapchain->Views)
	{
		OutSwapchain->Views =
			(VkImageView *)RendererArena->Push(sizeof(VkImageView) * OutSwapchain->ImageCount);
	}

	VK_CHECK(vkGetSwapchainImagesKHR(Context->Device.LogicalDevice, OutSwapchain->Handle,
									 &OutSwapchain->ImageCount, OutSwapchain->Images));

	// Get Views
	for (u32 Idx = 0; Idx < OutSwapchain->ImageCount; ++Idx)
	{
		VkImageViewCreateInfo ViewCreateInfo = {};
		ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ViewCreateInfo.image = OutSwapchain->Images[Idx];
		ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ViewCreateInfo.format = OutSwapchain->ImageFormat.format;
		ViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ViewCreateInfo.subresourceRange.levelCount = 1;
		ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ViewCreateInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(Context->Device.LogicalDevice, &ViewCreateInfo, Context->Allocator,
								   &OutSwapchain->Views[Idx]));
	}

	// Depth buffer creation
	if (!VulkanDeviceDetectDepthFormat(&Context->Device))
	{
		Context->Device.DepthFormat = VK_FORMAT_UNDEFINED;
		LogFatal("Failed to find a supported depth format");
	}

	VulkanImageCreate(Context, VK_IMAGE_TYPE_2D, SwapchainExtent.width, SwapchainExtent.height,
					  Context->Device.DepthFormat, VK_IMAGE_TILING_OPTIMAL,
					  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					  true, VK_IMAGE_ASPECT_DEPTH_BIT, &OutSwapchain->DepthAttachment);

	LogInfo("Swapchain created successfully");
}

void VulkanSwapchainDestroy(VulkanContext *Context, VulkanSwapchain *Swapchain)
{
	vkDeviceWaitIdle(Context->Device.LogicalDevice);

	VulkanImageDestroy(Context, &Swapchain->DepthAttachment);

	// NOTE: The images are created togheter with the Swapchain while we
	// create the views, so we also have to destroy them.
	for (u32 Idx = 0; Idx < Swapchain->ImageCount; ++Idx)
	{
		vkDestroyImageView(Context->Device.LogicalDevice, Swapchain->Views[Idx], Context->Allocator);
	}

	vkDestroySwapchainKHR(Context->Device.LogicalDevice, Swapchain->Handle, Context->Allocator);
}

void VulkanSwapchainRecreate(VulkanContext *Context, u32 Width, u32 Height, VulkanSwapchain *OutSwapchain)
{
	VulkanSwapchainDestroy(Context, OutSwapchain);
	// NOTE: We can pass a null arena since we know that if we are recreating the Swapchain
	// we had already allocated all the memory that we need.
	VulkanSwapchainCreate(Context, Width, Height, nullptr, OutSwapchain);
}

b8 VulkanSwapchainAcquireNextImageIndex(VulkanContext *Context, VulkanSwapchain *Swapchain,
										u64 TimeoutNs, VkSemaphore ImageAvailableSemaphore,
										VkFence Fence, u32 *OutImageIndex)
{
	VkResult Result = vkAcquireNextImageKHR(Context->Device.LogicalDevice, Swapchain->Handle, TimeoutNs,
											ImageAvailableSemaphore, Fence, OutImageIndex);

	if (Result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		VulkanSwapchainRecreate(Context, Context->FramebufferWidth, Context->FramebufferHeight, Swapchain);
		return false;
	}
	else if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
	{
		LogFatal("Failed to acquire swapchain image!");
		return false;
	}

	return true;
}

void VulkanSwapchainPresent(VulkanContext *Context, VulkanSwapchain *Swapchain,
							VkSemaphore RenderCompleteSemaphore,
							u32 PresentImageIndex)
{
	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = &RenderCompleteSemaphore;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &Swapchain->Handle;
	PresentInfo.pImageIndices = &PresentImageIndex;
	PresentInfo.pResults = 0;

	VkResult Result = vkQueuePresentKHR(Context->Device.PresentQueue, &PresentInfo);

	if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
	{
		VulkanSwapchainRecreate(Context, Context->FramebufferWidth, Context->FramebufferHeight, Swapchain);
	}
	else if (Result != VK_SUCCESS)
	{
		LogFatal("Failed to present swap chain image");
	}

	Context->CurrentFrame = (Context->CurrentFrame + 1) %
							Swapchain->ImageCount;
}