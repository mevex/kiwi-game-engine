#include "vulkan_types.h"
#include "vulkan_backend.h"
#include "core/logger.h"
#include "vulkan_device.h"

void VulkanSwapchain::Create(u32 Width, u32 Height, MemArena *RendererArena)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	AutoFreeArena ScratchArenaHandle(MemTag_Scratch);
	VulkanSwapchainSupport SwapchainSupport = {};
	if (!QuerySupport(Context->Device.PhysicalDevice, Context->Surface,
					  ScratchArenaHandle.Arena, SwapchainSupport))
	{
		LogWarning("Could not query swapchain support during Swapchain creation");
		KDebugBreak();
	}

	MaxFramesInFlight = 2; // Triple buffering

	b8 Found = false;
	for (u32 Idx = 0; Idx < SwapchainSupport.FormatCount; ++Idx)
	{
		VkSurfaceFormatKHR Format = SwapchainSupport.Formats[Idx];
		// Prefered format type
		if (Format.format == VK_FORMAT_B8G8R8A8_UNORM &&
			Format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			ImageFormat = Format;
			Found = true;
			break;
		}
	}

	if (!Found)
		ImageFormat = SwapchainSupport.Formats[0];

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

	// NOTE: We expect the min to be 2, so the ImgCount should be set to 3. This is because we want
	// to use triple buffering and so while 1 image is being presented we render on the other 2.
	// However, we safeguard by checking the max to be sure that 3 is actually supported.
	// Not really necessary on today's hardware, but better safe than sorry.
	ImageCount = SwapchainSupport.Capabilities.minImageCount + 1;
	if (SwapchainSupport.Capabilities.maxImageCount > 0 &&
		ImageCount > SwapchainSupport.Capabilities.maxImageCount)
	{
		ImageCount = SwapchainSupport.Capabilities.maxImageCount;
	}
	MaxFramesInFlight = (u8)ImageCount - 1;

	VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
	SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfo.surface = Context->Surface;
	SwapchainCreateInfo.minImageCount = ImageCount;
	SwapchainCreateInfo.imageFormat = ImageFormat.format;
	SwapchainCreateInfo.imageColorSpace = ImageFormat.colorSpace;
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
								  &Handle));

	Context->CurrentFrame = 0;

	// Get Images
	VK_CHECK(vkGetSwapchainImagesKHR(Context->Device.LogicalDevice, Handle, &ImageCount, 0));
	if (!Images)
	{
		Images = (VkImage *)RendererArena->Push(sizeof(VkImage) * ImageCount);
	}
	if (!Views)
	{
		Views =
			(VkImageView *)RendererArena->Push(sizeof(VkImageView) * ImageCount);
	}

	VK_CHECK(vkGetSwapchainImagesKHR(Context->Device.LogicalDevice, Handle, &ImageCount, Images));

	// Get Views
	for (u32 Idx = 0; Idx < ImageCount; ++Idx)
	{
		VkImageViewCreateInfo ViewCreateInfo = {};
		ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ViewCreateInfo.image = Images[Idx];
		ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ViewCreateInfo.format = ImageFormat.format;
		ViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ViewCreateInfo.subresourceRange.levelCount = 1;
		ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ViewCreateInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(Context->Device.LogicalDevice, &ViewCreateInfo, Context->Allocator,
								   &Views[Idx]));
	}

	// Depth buffer creation
	if (!VulkanDeviceDetectDepthFormat(&Context->Device))
	{
		Context->Device.DepthFormat = VK_FORMAT_UNDEFINED;
		LogFatal("Failed to find a supported depth format");
	}
	DepthAttachment.Create(VK_IMAGE_TYPE_2D, SwapchainExtent.width, SwapchainExtent.height,
						   Context->Device.DepthFormat, VK_IMAGE_TILING_OPTIMAL,
						   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						   true, VK_IMAGE_ASPECT_DEPTH_BIT);

	LogInfo("Swapchain created successfully");
}

void VulkanSwapchain::Destroy()
{
	VulkanContext *Context = &VulkanRenderer::Context;

	vkDeviceWaitIdle(Context->Device.LogicalDevice);

	DepthAttachment.Destroy();

	// NOTE: The images are created togheter with the Swapchain while we
	// create the views, so we also have to destroy them.
	for (u32 Idx = 0; Idx < ImageCount; ++Idx)
	{
		vkDestroyImageView(Context->Device.LogicalDevice, Views[Idx], Context->Allocator);
	}

	vkDestroySwapchainKHR(Context->Device.LogicalDevice, Handle, Context->Allocator);
}

void VulkanSwapchain::Recreate(u32 Width, u32 Height)
{
	Destroy();
	// NOTE: We can pass a null arena since we know that if we are recreating the Swapchain
	// we had already allocated all the memory that we need.
	Create(Width, Height, nullptr);
}

b8 VulkanSwapchain::AcquireNextImageIndex(u64 TimeoutNs, VkSemaphore ImageAvailableSemaphore,
										  VkFence Fence, u32 *OutImageIndex)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	VkResult Result = vkAcquireNextImageKHR(Context->Device.LogicalDevice, Handle, TimeoutNs,
											ImageAvailableSemaphore, Fence, OutImageIndex);

	if (Result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		VulkanSwapchain::Recreate(Context->FramebufferWidth, Context->FramebufferHeight);
		return false;
	}
	else if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
	{
		LogFatal("Failed to acquire swapchain image!");
		return false;
	}

	return true;
}

void VulkanSwapchain::Present(VkSemaphore RenderCompleteSemaphore, u32 PresentImageIndex)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = &RenderCompleteSemaphore;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &Handle;
	PresentInfo.pImageIndices = &PresentImageIndex;
	PresentInfo.pResults = 0;

	VkResult Result = vkQueuePresentKHR(Context->Device.PresentQueue, &PresentInfo);

	if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
	{
		Recreate(Context->FramebufferWidth, Context->FramebufferHeight);
	}
	else if (Result != VK_SUCCESS)
	{
		LogFatal("Failed to present swap chain image");
	}

	Context->CurrentFrame = (Context->CurrentFrame + 1) % ImageCount;
}

b8 VulkanSwapchain::QuerySupport(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface,
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