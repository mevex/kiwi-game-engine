#include "vulkan_backend.h"
#include "vulkan_platform.h"
#include "core/logger.h"
#include "core/kiwi_string.h"
#include "containers/karray.h"

VulkanContext VulkanRenderer::Context = {};

b8 VulkanRenderer::Initialize(const char *ApplicationName)
{
	// TODO: Custom allocator
	Context.Allocator = nullptr;

	// Setup Vulkan instance
	VkApplicationInfo AppInfo = {};
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pApplicationName = ApplicationName;
	AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.pEngineName = "Kiwi Engine";
	AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.apiVersion = VK_API_VERSION_1_4;

	// Obtain the list of required extensions
	KArray<char *> Extensions;
	Extensions.Create(3);
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
#ifdef KIWI_SLOW
	LogDebug("--- Vulkan validation layers enabled. Enumerating...");

	RequiredLayers.Create();
	RequiredLayers.Push("VK_LAYER_KHRONOS_validation");

	KArray<VkLayerProperties> AvailableLayers;
	u32 AvailableLayersCount = 0;
	VK_CHECK(vkEnumerateInstanceLayerProperties(&AvailableLayersCount, 0));
	AvailableLayers.Create(AvailableLayersCount);
	AvailableLayers.Length = AvailableLayersCount;
	VK_CHECK(vkEnumerateInstanceLayerProperties(&AvailableLayersCount, AvailableLayers.GetRawData()));

	for (u32 RequiredIndex = 0; RequiredIndex < RequiredLayers.Length; ++RequiredIndex)
	{
		b8 Found = false;
		for (u32 AvailableIndex = 0; AvailableIndex < AvailableLayers.Length; ++AvailableIndex)
		{
			VkLayerProperties Prop = AvailableLayers[AvailableIndex];
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
	InstanceCreateInfo.ppEnabledLayerNames = RequiredLayers.GetRawData();
	InstanceCreateInfo.enabledExtensionCount = (u32)Extensions.Length;
	InstanceCreateInfo.ppEnabledExtensionNames = Extensions.GetRawData();

	VK_CHECK(vkCreateInstance(&InstanceCreateInfo, Context.Allocator, &Context.Instance));
	LogInfo("Vulkan Instance created");

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

	LogInfo("Vulkan renderer initialized successfully");
	return true;
}

void VulkanRenderer::Terminate()
{
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
}

void VulkanRenderer::Resized()
{
}

// TODO: Error
#pragma warning(suppress : 4100)
b8 VulkanRenderer::BeginFrame(f32 DeltaTime)
{
	return true;
}

// TODO: Error
#pragma warning(suppress : 4100)
b8 VulkanRenderer::EndFrame(f32 DeltaTime)
{
	return true;
}

#ifdef KIWI_SLOW
#pragma warning(push)
#pragma warning(disable : 4100)
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
#pragma warning(pop)
#endif