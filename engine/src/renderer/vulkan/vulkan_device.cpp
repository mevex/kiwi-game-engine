#include "vulkan_device.h"
#include "core/logger.h"
#include "containers/karray.h"
#include "core/kiwi_string.h"
#include "core/kiwi_mem.h"

struct PhysicalDeviceRequirements
{
	// Queues
	b8 Graphics;
	b8 Present;
	b8 Transfer;
	b8 Compute;

	b8 SamplerAnisotropy;
	b8 DiscreteGPU;
	KArray<const char *> ExtensionNames;
};

struct PhysicalDeviceQueueFamilyInfo
{
	u32 GraphicsIndex;
	u32 PresentIndex;
	u32 TransferIndex;
	u32 ComputeIndex;
};

b8 VulkanDeviceCreate(VulkanContext *Context)
{
	AutoFreeArena ScratchArenaHandle = AutoFreeArena(MemTag_Scratch);

	if (!SelectPhysicalDevice(Context))
	{
		return false;
	}

	LogInfo("Creating logical device");
	// NOTE: Don't create additional queues for shared indices
	VulkanDevice &Device = Context->Device;
	KArray<u32> Indices;
	Indices.Create(ScratchArenaHandle.Arena);
	Indices.Push(Device.GraphicsIndex);
	if (Device.GraphicsIndex != Device.PresentIndex)
	{
		Indices.Push(Device.PresentIndex);
	}
	if (Device.GraphicsIndex != Device.TransferIndex)
	{
		Indices.Push(Device.TransferIndex);
	}
	if (Device.GraphicsIndex != Device.ComputeIndex)
	{
		Indices.Push(Device.ComputeIndex);
	}

	KArray<VkDeviceQueueCreateInfo> QueueCreateInfos;
	QueueCreateInfos.Create(ScratchArenaHandle.Arena, Indices.Length, Indices.Length);

	// NOTE: Since we know that we will create maximum 2 queues (graphics queues)
	// we need an array with 2 values, one for each queue, to pass to the
	// VkDeviceQueueCreateInfo structure
	f32 QueuePriorities[2] = {1.0f, 1.0f};

	for (u32 Idx = 0; Idx < QueueCreateInfos.Length; ++Idx)
	{
		VkDeviceQueueCreateInfo &Info = QueueCreateInfos[Idx];

		Info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		Info.queueFamilyIndex = Indices[Idx];
		Info.queueCount = 1;
		// TODO: we assume there are at least 2 graphics queue in this family.
		// Actually check.
		if (Indices[Idx] == Device.GraphicsIndex)
		{
			Info.queueCount = 2;
		}
		Info.flags = 0;
		Info.pNext = 0;
		Info.pQueuePriorities = QueuePriorities;
	}

	// Request device features
	// TODO: These should be driven by the engine configuration
	VkPhysicalDeviceFeatures DeviceFeatures = {};
	DeviceFeatures.samplerAnisotropy = VK_TRUE;

	const char *ExtensionName = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	VkDeviceCreateInfo DeviceCreateInfo = {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.queueCreateInfoCount = (u32)QueueCreateInfos.Length;
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.Elements;
	DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;
	DeviceCreateInfo.enabledExtensionCount = 1;
	DeviceCreateInfo.ppEnabledExtensionNames = &ExtensionName;
	// Those are deprecated
	// DeviceCreateInfo.enabledLayerCount = 0;
	// DeviceCreateInfo.ppEnabledLayerNames = 0;

	VK_CHECK(vkCreateDevice(Device.PhysicalDevice, &DeviceCreateInfo,
							Context->Allocator, &Device.LogicalDevice));

	LogInfo("Logical device created");

	// Get queues
	vkGetDeviceQueue(Device.LogicalDevice, Device.GraphicsIndex, 0, &Device.GraphicsQueue);
	vkGetDeviceQueue(Device.LogicalDevice, Device.PresentIndex, 0, &Device.PresentQueue);
	vkGetDeviceQueue(Device.LogicalDevice, Device.TransferIndex, 0, &Device.TransferQueue);
	vkGetDeviceQueue(Device.LogicalDevice, Device.ComputeIndex, 0, &Device.ComputeQueue);

	LogInfo("Queues obtained");

	// Create graphics command pool
	VkCommandPoolCreateInfo PoolCreateInfo = {};
	PoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	PoolCreateInfo.queueFamilyIndex = Device.GraphicsIndex;
	PoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VK_CHECK(vkCreateCommandPool(Device.LogicalDevice, &PoolCreateInfo, Context->Allocator,
								 &Device.GraphicsCommandPool));

	LogInfo("Craphics command pool Created");

	return true;
}

b8 SelectPhysicalDevice(VulkanContext *Context)
{
	AutoFreeArena ScratchArenaHandle = AutoFreeArena(MemTag_Scratch);

	LogInfo("Selecting physical device");
	// Querying for the appropriate physical device
	u32 PhysicalDeviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(Context->Instance, &PhysicalDeviceCount, nullptr));
	if (PhysicalDeviceCount == 0)
	{
		LogFatal("No device which support Vulkan found");
		return false;
	}

	KArray<VkPhysicalDevice> PhysicalDevices;
	PhysicalDevices.Create(ScratchArenaHandle.Arena, PhysicalDeviceCount, PhysicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(Context->Instance, &PhysicalDeviceCount,
										PhysicalDevices.Elements));

	// TODO: These should be driven by the engine configuration
	PhysicalDeviceRequirements Requirements = {};
	Requirements.Graphics = true;
	Requirements.Present = true;
	Requirements.Transfer = true;
	Requirements.Compute = true;
	Requirements.SamplerAnisotropy = true;
	Requirements.DiscreteGPU = true;
	Requirements.ExtensionNames.Create(ScratchArenaHandle.Arena);
	Requirements.ExtensionNames.Push(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	for (u32 Index = 0; Index < PhysicalDevices.Length; ++Index)
	{
		VkPhysicalDevice Device = PhysicalDevices[Index];

		VkPhysicalDeviceProperties Properties;
		VkPhysicalDeviceFeatures Features;
		VkPhysicalDeviceMemoryProperties MemoryProperties;

		// TODO: These are deprecated. Port them to the 2nd implementation
		// vkGetPhysicalDeviceProperties2(Device, &Properties);
		// vkGetPhysicalDeviceFeatures2(Device, &Features);
		// vkGetPhysicalDeviceMemoryProperties2(Device, &MemoryProperties);

		vkGetPhysicalDeviceProperties(Device, &Properties);
		vkGetPhysicalDeviceFeatures(Device, &Features);
		vkGetPhysicalDeviceMemoryProperties(Device, &MemoryProperties);

		PhysicalDeviceQueueFamilyInfo QueueInfo = {(u32)-1, (u32)-1, (u32)-1, (u32)-1};

		// Check if the GPU is descrete
		// HACK: Quick hack to turn the logic that RQUIRES a discrete GPU
		// into one that just checks if there is one,
		// this allows me to develop also on my poor laptop. SADGE
		for (u32 i = 0; i < PhysicalDevices.Length; ++i)
		{
			VkPhysicalDevice Dev = PhysicalDevices[i];
			VkPhysicalDeviceProperties Prop;
			vkGetPhysicalDeviceProperties(Dev, &Prop);
			// HACK: In order to go back to normal
			// remove the for loop and the above variables.

			if (Requirements.DiscreteGPU)
			{
				if (Prop.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					LogInfo("Device %d is not a discrete GPU", i);
					continue;
				}
			}
		}

		// Check the queues
		// TODO: This are deprecated. Port it to the 2nd implementation
		// vkGetPhysicalDeviceQueueFamilyProperties2(Device, &QueueFamilyCount, nullptr);
		u32 QueueFamilyCount = 0;
		KArray<VkQueueFamilyProperties> FamilyProperties;
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, nullptr);
		FamilyProperties.Create(ScratchArenaHandle.Arena, QueueFamilyCount, QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, FamilyProperties.Elements);

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

		LogInfo("Graphics | Present | Transfer | Compute | Name");
		LogInfo("       %d |       %d |        %d |       %d | %s",
				QueueInfo.GraphicsIndex, QueueInfo.PresentIndex,
				QueueInfo.TransferIndex, QueueInfo.ComputeIndex, Properties.deviceName);

		// Device extensions
		if (Requirements.ExtensionNames.Length)
		{
			u32 AvailableExtensionsCount = 0;
			KArray<VkExtensionProperties> ExtensionProperties;
			VK_CHECK(vkEnumerateDeviceExtensionProperties(Device, 0, &AvailableExtensionsCount, 0));
			if (AvailableExtensionsCount != 0)
			{
				ExtensionProperties.Create(ScratchArenaHandle.Arena, AvailableExtensionsCount, AvailableExtensionsCount);
				VK_CHECK(vkEnumerateDeviceExtensionProperties(Device, 0, &AvailableExtensionsCount,
															  ExtensionProperties.Elements));

				b8 Found = true;
				for (u32 ReqIdx = 0; ReqIdx < Requirements.ExtensionNames.Length && Found; ++ReqIdx)
				{
					Found = false;
					for (u32 AvIdx = 0; AvIdx < ExtensionProperties.Length; ++AvIdx)
					{
						if (KStr::Equal(Requirements.ExtensionNames[ReqIdx],
										ExtensionProperties[AvIdx].extensionName))
						{
							Found = true;
							break;
						}
					}

					if (!Found)
					{
						LogInfo("Could not find extension %s. Skipping.",
								Requirements.ExtensionNames[ReqIdx]);
						break;
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

		Context->Device.GraphicsIndex = QueueInfo.GraphicsIndex;
		Context->Device.PresentIndex = QueueInfo.PresentIndex;
		Context->Device.TransferIndex = QueueInfo.TransferIndex;
		Context->Device.ComputeIndex = QueueInfo.ComputeIndex;

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

void VulkanDeviceDestroy(VulkanContext *Context)
{
	LogInfo("Destroying command pools");
	vkDestroyCommandPool(Context->Device.LogicalDevice, Context->Device.GraphicsCommandPool,
						 Context->Allocator);

	LogInfo("Destroying logical device");
	if (Context->Device.LogicalDevice)
	{
		vkDestroyDevice(Context->Device.LogicalDevice, Context->Allocator);
		Context->Device.LogicalDevice = nullptr;
	}

	// Unset queue
	Context->Device.GraphicsQueue = 0;
	Context->Device.PresentQueue = 0;
	Context->Device.TransferQueue = 0;
	Context->Device.ComputeQueue = 0;

	// NOTE: there is not such a thing like destroying a physical device
	// but we can release all the resources that we obtained during creation
	LogInfo("Releasing physical device resources");
	Context->Device.PhysicalDevice = 0;

	Context->Device.GraphicsIndex = (u32)(-1);
	Context->Device.PresentIndex = (u32)(-1);
	Context->Device.TransferIndex = (u32)(-1);
	Context->Device.ComputeIndex = (u32)(-1);
}

b8 VulkanDeviceDetectDepthFormat(VulkanDevice *Device)
{
	// NOTE: The formats that we want in preference order
	VkFormat PreferredFormats[3] = {VK_FORMAT_D32_SFLOAT,
									VK_FORMAT_D32_SFLOAT_S8_UINT,
									VK_FORMAT_D24_UNORM_S8_UINT};

	u32 Flag = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
	for (u32 Idx = 0; Idx < ArrayCount(PreferredFormats); ++Idx)
	{
		VkFormatProperties Properties;
		vkGetPhysicalDeviceFormatProperties(Device->PhysicalDevice, PreferredFormats[Idx], &Properties);

		if (CheckFlags(Properties.linearTilingFeatures, Flag) ||
			CheckFlags(Properties.optimalTilingFeatures, Flag))
		{
			Device->DepthFormat = PreferredFormats[Idx];
			return true;
		}
	}

	return false;
}