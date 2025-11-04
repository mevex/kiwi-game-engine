#include "vulkan_backend.h"
#include "vulkan_platform.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_renderpass.h"
#include "vulkan_command_buffer.h"
#include "vulkan_framebuffer.h"
#include "vulkan_fence.h"
#include "core/logger.h"
#include "core/kiwi_string.h"
#include "containers/karray.h"

VulkanContext VulkanRenderer::Context = {};

b8 VulkanRenderer::Initialize(const char *ApplicationName, u32 Width, u32 Height)
{
	// TODO: Custom allocator
	Context.Allocator = nullptr;
	Context.FramebufferWidth = Width;
	Context.FramebufferHeight = Height;

	AutoFreeArena ScratchArenaHandle = AutoFreeArena(MemTag_Scratch);

	// Setup Vulkan instance
	VkApplicationInfo AppInfo = {};
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pApplicationName = ApplicationName;
	AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.pEngineName = "Kiwi Engine";
	AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.apiVersion = VK_API_VERSION_1_3;

	// Obtain the list of required extensions
	KArray<const char *> Extensions;
	Extensions.Create(ScratchArenaHandle.Arena);
	Extensions.Push(VK_KHR_SURFACE_EXTENSION_NAME); // generic surface ext
	VulkanPlatform::GetExtensions(Extensions);		// platform-specific extension(s)
#ifdef KIWI_SLOW
	Extensions.Push(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // debug utils

	LogDebug("--- Vulkan Required Extensions:");
	for (u32 i = 0; i < Extensions.Length; ++i)
	{
		LogDebug(Extensions[i]);
	}
	LogDebug("");
#endif

	// Obtain the list of required validation layers
	KArray<const char *> RequiredLayers;
	RequiredLayers.Create(ScratchArenaHandle.Arena);
#ifdef KIWI_SLOW
	LogDebug("--- Vulkan validation layers enabled. Enumerating...");

	RequiredLayers.Push("VK_LAYER_KHRONOS_validation");

	KArray<VkLayerProperties> AvailableLayers;
	u32 AvailableLayersCount = 0;
	VK_CHECK(vkEnumerateInstanceLayerProperties(&AvailableLayersCount, 0));
	AvailableLayers.Create(ScratchArenaHandle.Arena, AvailableLayersCount, AvailableLayersCount);
	VK_CHECK(vkEnumerateInstanceLayerProperties(&AvailableLayersCount, AvailableLayers.Elements));

	for (u32 RequiredIndex = 0; RequiredIndex < RequiredLayers.Length; ++RequiredIndex)
	{
		b8 Found = false;
		for (u32 AvailableIndex = 0; AvailableIndex < AvailableLayers.Length; ++AvailableIndex)
		{
			if (KStr::Equal(RequiredLayers[RequiredIndex], AvailableLayers[AvailableIndex].layerName))
			{
				Found = true;
				LogDebug("%s found", RequiredLayers[RequiredIndex]);
				break;
			}
		}

		if (!Found)
		{
			LogFatal("Required validation layer is missing: %s", RequiredLayers[RequiredIndex]);
			return false;
		}
	}

	LogDebug("Found all validation layers required");
	LogDebug("");
#endif

	VkInstanceCreateInfo InstanceCreateInfo = {};
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pApplicationInfo = &AppInfo;
	InstanceCreateInfo.enabledLayerCount = (u32)RequiredLayers.Length;
	InstanceCreateInfo.ppEnabledLayerNames = RequiredLayers.Elements;
	InstanceCreateInfo.enabledExtensionCount = (u32)Extensions.Length;
	InstanceCreateInfo.ppEnabledExtensionNames = Extensions.Elements;

	VK_CHECK(vkCreateInstance(&InstanceCreateInfo, Context.Allocator, &Context.Instance));
	LogInfo("Vulkan Instance created");

	u32 InstanceVersion;
	VK_CHECK(vkEnumerateInstanceVersion(&InstanceVersion));
	LogInfo("Instance Version: %d.%d.%d", VK_API_VERSION_MAJOR(InstanceVersion),
			VK_API_VERSION_MINOR(InstanceVersion), VK_API_VERSION_PATCH(InstanceVersion));

#ifdef KIWI_SLOW
	LogDebug("--- Creating Vulkan Debugger");

	VkDebugUtilsMessengerCreateInfoEXT DebugMessengerInfo = {};
	DebugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	DebugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
										 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
										 VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
	DebugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	DebugMessengerInfo.pfnUserCallback = VulkanDebugCallback;

	// NOTE: since we are using an extension we nee to manually
	// load the function that allows us to create the debugger
	PFN_vkCreateDebugUtilsMessengerEXT CreateDebuggerFunc =
		(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Context.Instance,
																  "vkCreateDebugUtilsMessengerEXT");
	AssertMsg(CreateDebuggerFunc, "Cannot get the Create Debugger function");

	VK_CHECK(CreateDebuggerFunc(Context.Instance, &DebugMessengerInfo,
								Context.Allocator, &Context.DebugMessenger));

	LogDebug("Vulkan Debugger successfully created");

#endif

	if (!VulkanPlatform::CreateSurface(PlatState, &Context))
	{
		LogFatal("Could not create Vulkan surface");
		return false;
	}

	if (!VulkanDeviceCreate(&Context))
	{
		LogFatal("Could not create Vulkan device");
		return false;
	}

	VulkanSwapchainCreate(&Context, Context.FramebufferWidth, Context.FramebufferHeight,
						  Arena, &Context.Swapchain);

	VulkanRenderPassCreate(&Context,
						   0, 0, Context.FramebufferWidth, Context.FramebufferHeight,
						   1.0f, 0.47f, 0.0f, 1.0f, 1.0f, 0, &Context.MainRenderPass);

	// TODO: Are we allocating on the right arena?
	Context.Swapchain.Framebuffers.Create(Arena, Context.Swapchain.ImageCount, Context.Swapchain.ImageCount);
	RegenerateFramebuffers(&Context.Swapchain, &Context.MainRenderPass);

	CreateCommandBuffers();

	// Create sync objects
	// TODO: Are we allocating on the right arena?
	Context.ImageAvailableSemaphores.Create(Arena, Context.Swapchain.MaxFramesInFlight,
											Context.Swapchain.MaxFramesInFlight);
	Context.QueueCompleteSemaphores.Create(Arena, Context.Swapchain.MaxFramesInFlight,
										   Context.Swapchain.MaxFramesInFlight);
	Context.InFlightFences.Create(Arena, Context.Swapchain.MaxFramesInFlight,
								  Context.Swapchain.MaxFramesInFlight);

	for (u32 Idx = 0; Idx < Context.Swapchain.MaxFramesInFlight; ++Idx)
	{
		VkSemaphoreCreateInfo SemaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
		vkCreateSemaphore(Context.Device.LogicalDevice, &SemaphoreCreateInfo, Context.Allocator,
						  &Context.ImageAvailableSemaphores[Idx]);
		vkCreateSemaphore(Context.Device.LogicalDevice, &SemaphoreCreateInfo, Context.Allocator,
						  &Context.QueueCompleteSemaphores[Idx]);

		// NOTE: Creating the fence in a signaled state.
		VulkanFenceCreate(&Context, true, &Context.InFlightFences[Idx]);
	}

	// TODO: Are we allocating on the right arena?
	Context.ImagesInFlight.Create(Arena, Context.Swapchain.ImageCount, Context.Swapchain.ImageCount);

	LogInfo("Vulkan renderer initialized successfully");
	return true;
}

void VulkanRenderer::Terminate()
{
	// NOTE: Before terminating we want to be sure that all the graphics operations are over
	vkDeviceWaitIdle(Context.Device.LogicalDevice);

	for (u32 Idx = 0; Idx < Context.Swapchain.MaxFramesInFlight; ++Idx)
	{
		if (Context.ImageAvailableSemaphores[Idx])
		{
			vkDestroySemaphore(Context.Device.LogicalDevice, Context.ImageAvailableSemaphores[Idx],
							   Context.Allocator);
		}
		if (Context.QueueCompleteSemaphores[Idx])
		{
			vkDestroySemaphore(Context.Device.LogicalDevice, Context.QueueCompleteSemaphores[Idx],
							   Context.Allocator);
		}

		// NOTE: Creating the fence in a signaled state.
		VulkanFenceDestroy(&Context, &Context.InFlightFences[Idx]);
	}

	DestroyCommandBuffers();

	for (u32 Idx = 0; Idx < Context.Swapchain.ImageCount; ++Idx)
	{
		VulkanFramebufferDestroy(&Context, &Context.Swapchain.Framebuffers[Idx]);
	}

	VulkanRenderPassDestroy(&Context, &Context.MainRenderPass);

	VulkanSwapchainDestroy(&Context, &Context.Swapchain);

	LogDebug("Destroying Vulkan device");
	VulkanDeviceDestroy(&Context);

	LogDebug("Destroying Vulkan surface");
	if (Context.Surface)
	{
		vkDestroySurfaceKHR(Context.Instance, Context.Surface, Context.Allocator);
		Context.Surface = 0;
	}

#ifdef KIWI_SLOW
	LogDebug("Destroying Vulkan Debugger");
	if (Context.DebugMessenger)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebuggerFunc =
			(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Context.Instance,
																	   "vkDestroyDebugUtilsMessengerEXT");
		DestroyDebuggerFunc(Context.Instance, Context.DebugMessenger, Context.Allocator);
	}
#endif

	LogDebug("Destroyng the Vulkan Instance");
	vkDestroyInstance(Context.Instance, Context.Allocator);

	Arena->Clear();
}

void VulkanRenderer::Resized(u16 Width, u16 Height)
{
	CachedFramebufferWidth = Width;
	CachedFramebufferHeight = Height;
	Context.FramebufferSizeGeneration++;

	LogInfo("Vulkan renderer backend resized. W: %i, H: %i, Gen: %llu",
			Width, Height, Context.FramebufferSizeGeneration);
}

// TODO: Error
SUPPRESS_WARNING(4100)
b8 VulkanRenderer::BeginFrame(f32 DeltaTime)
{
	return true;
}

// TODO: Error
SUPPRESS_WARNING(4100)
b8 VulkanRenderer::EndFrame(f32 DeltaTime)
{
	return true;
}

#ifdef KIWI_SLOW
// NOTE: we simply don't need some of the parameters
DISABLE_WARNING_PUSH
DISABLE_WARNING(4100)
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
												   VkDebugUtilsMessageTypeFlagsEXT MessageTypes,
												   const VkDebugUtilsMessengerCallbackDataEXT *CallbackData,
												   void *UserData)
{
	switch (MessageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
	{
		LogError(CallbackData->pMessage);
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
	{
		LogWarning(CallbackData->pMessage);
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
	{
		LogInfo(CallbackData->pMessage);
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
	{
		LogTrace(CallbackData->pMessage);
		break;
	}
	}
	return VK_FALSE;
}
DISABLE_WARNING_POP
#endif

void VulkanRenderer::CreateCommandBuffers()
{
	// Create a command buffer for each swapchain image
	if (Context.GraphicsCommandBuffers.Length == 0)
	{
		Context.GraphicsCommandBuffers.Create(Arena, Context.Swapchain.ImageCount,
											  Context.Swapchain.ImageCount);
	}

	for (u32 Idx = 0; Idx < Context.Swapchain.ImageCount; ++Idx)
	{
		if (Context.GraphicsCommandBuffers[Idx].Handle)
		{
			VulkanCommandBufferFree(&Context, Context.Device.GraphicsCommandPool,
									&Context.GraphicsCommandBuffers[Idx]);
		}
		MemSystem::Zero(&Context.GraphicsCommandBuffers[Idx], Context.GraphicsCommandBuffers.Stride);

		VulkanCommandBufferAllocate(&Context, Context.Device.GraphicsCommandPool, true,
									&Context.GraphicsCommandBuffers[Idx]);
	}

	LogInfo("Graphics command buffer created");
}

void VulkanRenderer::DestroyCommandBuffers()
{
	for (u32 Idx = 0; Idx < Context.Swapchain.ImageCount; ++Idx)
	{
		if (Context.GraphicsCommandBuffers[Idx].Handle)
		{
			VulkanCommandBufferFree(&Context, Context.Device.GraphicsCommandPool,
									&Context.GraphicsCommandBuffers[Idx]);
		}
	}
}

void VulkanRenderer::RegenerateFramebuffers(VulkanSwapchain *Swapchain, VulkanRenderPass *RenderPass)
{
	for (u32 Idx = 0; Idx < Swapchain->ImageCount; ++Idx)
	{
		// TODO: Make this dynamic
		u32 AttachmentCount = 2;
		VkImageView Attachments[] = {Swapchain->Views[Idx], Swapchain->DepthAttachment.View};

		VulkanFramebufferCreate(&Context, RenderPass, Context.FramebufferWidth, Context.FramebufferHeight,
								AttachmentCount, Attachments, &Swapchain->Framebuffers[Idx]);
	}
}