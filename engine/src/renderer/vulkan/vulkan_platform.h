#pragma once

#include "defines.h"
#include "containers/karray.h"

struct PlatformState;
struct VulkanContext;

namespace VulkanPlatform
{
	void GetExtensions(KArray<const char *> &Extensions);

	b8 CreateSurface(PlatformState *PlatState, VulkanContext *VkContext);
}