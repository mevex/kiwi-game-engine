#include "vulkan_types.h"
#include "vulkan_backend.h"
#include "core/logger.h"

void VulkanFence::Create(b8 CreateSignaled)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	IsSignaled = CreateSignaled;

	VkFenceCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	CreateInfo.flags = IsSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VK_CHECK(vkCreateFence(Context->Device.LogicalDevice, &CreateInfo, Context->Allocator,
						   &Handle));
}

void VulkanFence::Destroy()
{
	VulkanContext *Context = &VulkanRenderer::Context;

	if (Handle)
	{
		vkDestroyFence(Context->Device.LogicalDevice, Handle, Context->Allocator);
		Handle = 0;
	}
	IsSignaled = false;
}

b8 VulkanFence::Wait(u64 TimeoutNS)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	if (IsSignaled)
	{
		return true;
	}
	else
	{
		VkResult Result = vkWaitForFences(Context->Device.LogicalDevice, 1, &Handle,
										  true, TimeoutNS);
		switch (Result)
		{
		case VK_SUCCESS:
		{
			IsSignaled = true;
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

void VulkanFence::Reset()
{
	VulkanContext *Context = &VulkanRenderer::Context;

	if (IsSignaled)
	{
		VK_CHECK(vkResetFences(Context->Device.LogicalDevice, 1, &Handle));
		IsSignaled = false;
	}
}