#include "vulkan_backend.h"
#include "core/logger.h"

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

	VkInstanceCreateInfo InstanceCreateInfo = {};
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pApplicationInfo = &AppInfo;
	// InstanceCreateInfo.enabledLayerCount = ;
	// InstanceCreateInfo.ppEnabledLayerNames = ;
	// InstanceCreateInfo.enabledExtensionCount = ;
	// InstanceCreateInfo.ppEnabledExtensionNames = ;

	VkResult Result = vkCreateInstance(&InstanceCreateInfo, Context.Allocator, &Context.Instance);
	if (Result != VK_SUCCESS)
	{
		LogError("vkCreateInstance failed with result: %u", Result);
		return false;
	}

	LogInfo("Vulkan renderer initialized successfully");
	return true;
}

void VulkanRenderer::Terminate()
{
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