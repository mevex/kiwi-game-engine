#include "vulkan_fence.h"
#include "core/logger.h"

void VulkanFenceCreate(VulkanContext *Context, b8 CreateSignaled, VulkanFence *OutFence)
{
	OutFence->IsSignaled = CreateSignaled;

	VkFenceCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	CreateInfo.flags = OutFence->IsSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VK_CHECK(vkCreateFence(Context->Device.LogicalDevice, &CreateInfo, Context->Allocator,
						   &OutFence->Handle));
}

void VulkanFenceDestroy(VulkanContext *Context, VulkanFence *Fence)
{
	if (Fence->Handle)
	{
		vkDestroyFence(Context->Device.LogicalDevice, Fence->Handle, Context->Allocator);
		Fence->Handle = 0;
	}
	Fence->IsSignaled = false;
}

b8 VulkanFenceWait(VulkanContext *Context, VulkanFence *Fence, u64 TimeoutNS)
{
	if (Fence->IsSignaled)
	{
		return true;
	}
	else
	{
		VkResult Result = vkWaitForFences(Context->Device.LogicalDevice, 1, &Fence->Handle,
										  true, TimeoutNS);
		switch (Result)
		{
		case VK_SUCCESS:
		{
			Fence->IsSignaled = true;
			return true;
		}
		case VK_TIMEOUT:
		{
			LogWarning("Vulkan Fence Wait: Timed out");
			break;
		}
		case VK_ERROR_DEVICE_LOST:
		{
			LogError("Vulkan Fence Wait: VK_ERROR_DEVICE_LOST");
			break;
		}
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		{
			LogError("Vulkan Fence Wait: VK_ERROR_OUT_OF_HOST_MEMORY");
			break;
		}
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		{
			LogError("Vulkan Fence Wait: VK_ERROR_OUT_OF_DEVICE_MEMORY");
			break;
		}
		default:
		{
			LogError("Vulkan Fence Wait: An unknown error has occurred");
			break;
		}
		}
	}
	return false;
}

void VulkanFenceReset(VulkanContext *Context, VulkanFence *Fence)
{
	if (Fence->IsSignaled)
	{
		VK_CHECK(vkResetFences(Context->Device.LogicalDevice, 1, &Fence->Handle));
		Fence->IsSignaled = false;
	}
}