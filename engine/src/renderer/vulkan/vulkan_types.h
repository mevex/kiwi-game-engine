#pragma once

#include "containers/karray.h"
#include "vulkan_utils.h"

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
	void Create(VkImageType Type, u32 InWidth, u32 InHeight, VkFormat Format, VkImageTiling Tiling,
				VkImageUsageFlags Usage, VkMemoryPropertyFlags MemoryFlags, b8 CreateView,
				VkImageAspectFlags ViewAspectFlags);

	void Destroy();

	void CreateView(VkFormat Format, VkImageAspectFlags AspectFlags);

	u32 FindMemoryTypeIndex(u32 TypeFilter, u32 PropertyFlags);

	VkImage Handle;
	VkImageView View;
	VkDeviceMemory Memory;
	u32 Width;
	u32 Height;
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
	void Allocate(VkCommandPool Pool, b8 IsPrimary);

	void Free(VkCommandPool Pool);

	void Begin(b8 IsSingleUse, b8 IsRenderPassContinue, b8 IsSimultaneousUse);

	void End();

	void UpdateSubmitted();

	void Reset();

	// Helper functions that allow to quickly get and use a single use command buffer
	void AllocateAndBeginSingleUse(VkCommandPool Pool);

	void EndSingleUse(VkCommandPool Pool, VkQueue Queue);

	VkCommandBuffer Handle;
	VulkanCommandBufferState State;
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
	void Create(u32 InX, u32 InY, u32 InW, u32 InH, f32 InR, f32 InG, f32 InB, f32 InA,
				f32 InDepth, u32 InStencil);

	void Destroy();

	void Begin(VulkanCommandBuffer *CommandBuffer, VkFramebuffer FrameBuffer);

	void End(VulkanCommandBuffer *CommandBuffer);

	VkRenderPass Handle;
	i32 x, y, w, h;
	f32 r, g, b, a;

	f32 Depth;
	u32 Stencil;

	VulkanRenderPassState State;
};

struct VulkanFramebuffer
{
	void Create(VulkanRenderPass *RenderPass, u32 Width, u32 Height, u32 AttachmentCount,
				VkImageView *Attachments);

	void Destroy();

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

struct VulkanFence
{
	void Create(b8 CreateSignaled);

	void Destroy();

	b8 Wait(u64 TimeoutNS);

	void Reset();

	VkFence Handle;
	b8 IsSignaled;
};

struct VulkanContext
{
	u32 FramebufferWidth;
	u32 FramebufferHeight;

	// NOTE: these counters are used for synchronization purposes.
	// If the first doesn't match the second, it means that a new
	// framebuffer must be generated
	u64 FramebufferSizeGeneration;
	u64 FramebufferSizeLastGeneration;

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

	KArray<VkSemaphore> ImageAvailableSemaphores;
	KArray<VkSemaphore> QueueCompleteSemaphores;
	KArray<VulkanFence> InFlightFences;

	KArray<VulkanFence *> ImagesInFlight;

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