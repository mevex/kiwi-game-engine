#pragma once

#include "defines.h"

// NOTE: these are here in case we get really really
// carried away with the graphics programming.
// DirectX could be interesting one day?
enum RendererBackendType
{
	RendererBackendType_Vulkan,
	RendererBackendType_DirectX,
	RendererBackendType_OpenGL
};

struct RenderPacket
{
	f32 DeltaTime;
};