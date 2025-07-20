#include "vulkan_device.h"
#include "core/logger.h"
#include "containers/karray.h"
#include "core/kiwi_string.h"

struct PhysicalDeviceRequirements
{
	// Queues
	b8 Graphics;
	b8 Present;
	b8 Compute;
	b8 Transfer;

	KArray<char *> ExtentionNames;
	b8 SamplerAnisotropy;
	b8 DiscreteGPU;
};

struct PhysicalDeviceQueueFamilyInfo
{
	u32 GraphicsIndex;
	u32 PresentIndex;
	u32 ComputeIndex;
	u32 TransferIndex;
};

b8 VulkanDeviceCreate(VulkanContext *Context)
{
	// Querying for the appropriate physical device
	u32 PhysicalDeviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(Context->Instance, &PhysicalDeviceCount, nullptr));
	if (PhysicalDeviceCount == 0)
	{
		LogFatal("No device which support Vulkan found");
		return false;
	}

	KArray<VkPhysicalDevice> PhysicalDevices;
	PhysicalDevices.Create(PhysicalDeviceCount, PhysicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(Context->Instance, &PhysicalDeviceCount,
										PhysicalDevices.GetRawData()));

	// TODO: These should be driven by the engine configuration
	PhysicalDeviceRequirements Requirements = {};
	Requirements.Graphics = true;
	Requirements.Present = true;
	Requirements.Compute = true;
	Requirements.Transfer = true;
	Requirements.SamplerAnisotropy = true;
	Requirements.DiscreteGPU = true;
	Requirements.ExtentionNames.Push(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	for (u32 Index = 0; Index < PhysicalDevices.Length; ++Index)
	{
		VkPhysicalDevice Device = PhysicalDevices[Index];

		VkPhysicalDeviceProperties Properties;
		VkPhysicalDeviceFeatures Features;
		VkPhysicalDeviceMemoryProperties MemoryProperties;

		vkGetPhysicalDeviceProperties(Device, &Properties);
		vkGetPhysicalDeviceFeatures(Device, &Features);
		vkGetPhysicalDeviceMemoryProperties(Device, &MemoryProperties);

		PhysicalDeviceQueueFamilyInfo QueueInfo = {(u32)-1, (u32)-1, (u32)-1, (u32)-1};

		// Check if the GPU is descrete
		if (Requirements.DiscreteGPU)
		{
			if (Properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				LogInfo("Device %d is not a discrete GPU. Skipping.", Index);
				continue;
			}
		}

		// Check the queues
		u32 QueueFamilyCount = 0;
		KArray<VkQueueFamilyProperties> FamilyProperties;
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, nullptr);
		FamilyProperties.Create(QueueFamilyCount, QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, FamilyProperties.GetRawData());

		u8 MinTransferScore = 255;
		for (u32 FamIdx = 0; FamIdx < FamilyProperties.Length; ++FamIdx)
		{
			u8 CurrentTransferScore = 0;

			// Check graphics queue
			if (FamilyProperties[FamIdx].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				QueueInfo.GraphicsIndex = FamIdx;
				++CurrentTransferScore;
			}
			// Check compute queue
			if (FamilyProperties[FamIdx].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				QueueInfo.ComputeIndex = FamIdx;
				++CurrentTransferScore;
			}
			// Check transfer queue
			if (FamilyProperties[FamIdx].queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				if (CurrentTransferScore <= MinTransferScore)
				{
					// NOTE: We would prefer to have a dedicated transfer Queue
					// Using the score we pick a queue that is less "crowded"
					// that hopefully ends up being dedicated
					// TODO: That's not the smartest way to do this, but it works for now
					MinTransferScore = CurrentTransferScore;
					QueueInfo.TransferIndex = FamIdx;
				}
			}

			// Check present queue
			VkBool32 SupportsPresent = VK_FALSE;
			VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(Device, FamIdx,
														  Context->Surface, &SupportsPresent))
			if (SupportsPresent)
			{
				QueueInfo.PresentIndex = FamIdx;
			}
		}
		LogInfo("Graphics | Present | Compute | Transfer | Name");
		LogInfo("       %d |       %d |        %d |       %d | %s",
				QueueInfo.GraphicsIndex, QueueInfo.PresentIndex,
				QueueInfo.ComputeIndex, QueueInfo.TransferIndex, Properties.deviceName);

		VulkanSwapchainSupport SwapchainSupport = {};
		if ((!Requirements.Graphics || (Requirements.Graphics && QueueInfo.GraphicsIndex != (u32)-1)) &&
			(!Requirements.Present || (Requirements.Present && QueueInfo.PresentIndex != (u32)-1)) &&
			(!Requirements.Compute || (Requirements.Compute && QueueInfo.ComputeIndex != (u32)-1)) &&
			(!Requirements.Transfer || (Requirements.Transfer && QueueInfo.TransferIndex != (u32)-1)))
		{
			VulkanDeviceQuerySwapchainSupport(Device, Context->Surface, SwapchainSupport);

			if (SwapchainSupport.FormatCount < 1 || SwapchainSupport.PresentModeCount < 1)
			{
				SwapchainSupport.Formats.Destroy();
				SwapchainSupport.PresentModes.Destroy();

				LogInfo("Required swapchain support not present on device %d. Skipping.");
				continue;
			}
		}

		// Device extensions
		if (Requirements.ExtentionNames.Length)
		{
			u32 AvailableExtensionsCount = 0;
			KArray<VkExtensionProperties> ExtensionProperties;
			VK_CHECK(vkEnumerateDeviceExtensionProperties(Device, 0, &AvailableExtensionsCount, 0));
			if (AvailableExtensionsCount != 0)
			{
				ExtensionProperties.Create(AvailableExtensionsCount, AvailableExtensionsCount);
				VK_CHECK(vkEnumerateDeviceExtensionProperties(Device, 0, &AvailableExtensionsCount,
															  ExtensionProperties.GetRawData()));

				b8 Found = true;
				for (u32 ReqIdx = 0; ReqIdx < Requirements.ExtentionNames.Length && Found; ++ReqIdx)
				{
					Found = false;
					for (u32 AvIdx = 0; AvIdx < ExtensionProperties.Length; ++AvIdx)
					{
						if (KStr::Equal(Requirements.ExtentionNames[ReqIdx],
										ExtensionProperties[AvIdx].extensionName))
						{
							Found = true;
							break;
						}
					}

					if (!Found)
					{
						LogInfo("Could not find extension %s. Skipping.",
								Requirements.ExtentionNames[ReqIdx]);
					}
				}

				if (!Found)
				{
					continue;
				}
			}
		}

		if (Requirements.SamplerAnisotropy && !Features.samplerAnisotropy)
		{
			LogInfo("Device %d does not support sampler anisotropy. Skipping.");
			continue;
		}

		// Device meets all the requirements
		Context->Device.PhysicalDevice = Device;
		Context->Device.SwapchainSupport = SwapchainSupport;

		Context->Device.GraphicsIndex = QueueInfo.GraphicsIndex;
		Context->Device.PresentIndex = QueueInfo.PresentIndex;
		Context->Device.ComputeIndex = QueueInfo.ComputeIndex;
		Context->Device.TransferIndex = QueueInfo.TransferIndex;

		Context->Device.Properties = Properties;
		Context->Device.Features = Features;
		Context->Device.MemoryProperties = MemoryProperties;

		// Log some informations
		LogInfo("Selected device: %s", Properties.deviceName);
		switch (Properties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		{
			LogInfo("GPU type is Integrated");
		}
		break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		{
			LogInfo("GPU type is Discrete");
		}
		break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		{
			LogInfo("GPU type is Virtual");
		}
		break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
		{
			LogInfo("GPU type is CPU");
		}
		break;
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		default:
		{
			LogInfo("GPU type is Unknown");
		}
		}

		LogInfo("GPU Driver version: %d.%d.%d",
				VK_VERSION_MAJOR(Properties.driverVersion),
				VK_VERSION_MINOR(Properties.driverVersion),
				VK_VERSION_PATCH(Properties.driverVersion));

		LogInfo("Vulkan API version: %d.%d.%d",
				VK_VERSION_MAJOR(Properties.apiVersion),
				VK_VERSION_MINOR(Properties.apiVersion),
				VK_VERSION_PATCH(Properties.apiVersion));

		for (u32 HeapIdx = 0; HeapIdx < MemoryProperties.memoryHeapCount; ++HeapIdx)
		{
			f32 MemorySizeGib = (f32)(ToGiB(MemoryProperties.memoryHeaps[HeapIdx].size));
			if (MemoryProperties.memoryHeaps[HeapIdx].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				LogInfo("Local GPU memory: %.2f GiB", MemorySizeGib);
			}
			else
			{
				LogInfo("Shared System memory: %.2f GiB", MemorySizeGib);
			}
		}

		LogInfo("Physical Device Selected");
		return true;
	}

	LogError("No physical device that meets the requirement found");
	return false;
}

void VulkanDeviceQuerySwapchainSupport(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface,
									   VulkanSwapchainSupport &OutSwapchainSupport)
{
	// Surface capabilities
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface,
													   &OutSwapchainSupport.Capabilities));

	// Surface formats
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface,
												  &OutSwapchainSupport.FormatCount, nullptr));
	if (OutSwapchainSupport.FormatCount != 0)
	{
		OutSwapchainSupport.Formats.Create(OutSwapchainSupport.FormatCount,
										   OutSwapchainSupport.FormatCount);
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface,
													  &OutSwapchainSupport.FormatCount,
													  OutSwapchainSupport.Formats.GetRawData()));
	}

	// Present modes
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface,
													   &OutSwapchainSupport.PresentModeCount, nullptr));
	if (OutSwapchainSupport.PresentModeCount != 0)
	{
		OutSwapchainSupport.PresentModes.Create(OutSwapchainSupport.PresentModeCount,
												OutSwapchainSupport.PresentModeCount);
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface,
														   &OutSwapchainSupport.PresentModeCount,
														   OutSwapchainSupport.PresentModes.GetRawData()));
	}
}

void VulkanDeviceDestroy(VulkanContext *Context)
{
	// TODO:
	Context;
}