#pragma once

#include "renderer_types.h"

struct PlatformState;

class RendererBackend
{
public:
	static b8 Create(RendererBackendType Type, PlatformState *PlatState,
					 const char *ApplicationName, RendererBackend **OutRendererBackend);

	static void Destroy(RendererBackend *RendererBackend);

	virtual b8 Initialize(const char *ApplicationName) = 0;

	virtual void Terminate() = 0;

	virtual void Resized() = 0;

	virtual b8 BeginFrame(f32 DeltaTime) = 0;

	virtual b8 EndFrame(f32 DeltaTime) = 0;

	RendererBackendType Type;

	PlatformState *PlatState;
	u64 FrameCount;
};
