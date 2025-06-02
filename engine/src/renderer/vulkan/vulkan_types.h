#pragma once

#include "defines.h"
#include <vulkan/vulkan.h>

struct VulkanContext
{
	VkInstance Instance;
	VkAllocationCallbacks *Allocator;
};