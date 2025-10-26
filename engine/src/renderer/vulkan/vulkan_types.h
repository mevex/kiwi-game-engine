#pragma once

#include "defines.h"
#include "containers/karray.h"
#include <vulkan/vulkan.h>

#define VK_CHECK(Expression) Assert((Expression) == VK_SUCCESS)

struct VulkanDevice
{
	VkPhysicalDevice PhysicalDevice;
	VkDevice LogicalDevice;

	u32 GraphicsIndex;
	u32 PresentIndex;
	u32 TransferIndex;
	// NOTE: We will probably not use compute at first
	u32 ComputeIndex;

	VkQueue GraphicsQueue;
	VkQueue PresentQueue;
	VkQueue TransferQueue;
	VkQueue ComputeQueue;

	VkCommandPool GraphicsCommandPool;

	VkPhysicalDeviceProperties Properties;
	VkPhysicalDeviceFeatures Features;
	VkPhysicalDeviceMemoryProperties MemoryProperties;

	VkFormat DepthFormat;
};

struct VulkanImage
{
	VkImage Handle;
	VkImageView View;
	VkDeviceMemory Memory;
	u32 Width;
	u32 Height;
};

enum VulkanRenderPassState
{
	RPS_NotAllocated,
	RPS_Ready,
	RPS_Recording,
	RPS_InRenderPass,
	RPS_RecordingEnded,
	RPS_Submitted,
};

struct VulkanRenderPass
{
	VkRenderPass Handle;
	i32 x, y, w, h;
	f32 r, g, b, a;

	f32 Depth;
	u32 Stencil;

	VulkanRenderPassState State;
};

enum VulkanCommandBufferState
{
	CBS_NotAllocated,
	CBS_Ready,
	CBS_Recording,
	CBS_InRenderPass,
	CBS_RecordingEnded,
	CBS_Submitted,
};

struct VulkanCommandBuffer
{
	VkCommandBuffer Handle;
	VulkanCommandBufferState State;
};

struct VulkanFramebuffer
{
	VkFramebuffer Handle;
	u32 AttachmentCount;
	VkImageView *Attachments;
	VulkanRenderPass *RenderPass;
};

struct VulkanSwapchain
{
	VkSurfaceFormatKHR ImageFormat;
	u8 MaxFramesInFlight;
	VkSwapchainKHR Handle;
	u32 ImageCount;
	VkImage *Images;
	VkImageView *Views;

	VulkanImage DepthAttachment;

	KArray<VulkanFramebuffer> Framebuffers;
};

struct VulkanContext
{
	u32 FramebufferWidth;
	u32 FramebufferHeight;

	VkInstance Instance;
	VkAllocationCallbacks *Allocator;
	VkSurfaceKHR Surface;

	VulkanDevice Device;

	VulkanSwapchain Swapchain;
	u32 ImageIndex;
	u32 CurrentFrame;
	b8 RecreatingSwapchain;

	VulkanRenderPass MainRenderPass;

	KArray<VulkanCommandBuffer> GraphicsCommandBuffers;

#ifdef KIWI_SLOW
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif
};

#ifdef KIWI_SLOW
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
												   VkDebugUtilsMessageTypeFlagsEXT MessageTypes,
												   const VkDebugUtilsMessengerCallbackDataEXT *CallbackData,
												   void *UserData);
#endif