#pragma once

#include "renderer_types.h"

class MemArena;
struct PlatformState;

class RendererBackend
{
public:
	static b8 Create(RendererBackendType Type, PlatformState *PlatState, const char *ApplicationName,
					 u32 Width, u32 Height, RendererBackend **OutRendererBackend);

	static void Destroy(RendererBackend *RendererBackend);

	virtual b8 Initialize(const char *ApplicationName, u32 Width, u32 Height) = 0;

	virtual void Terminate() = 0;

	virtual void Resized(u16 Width, u16 Height) = 0;

	virtual b8 BeginFrame(f32 DeltaTime) = 0;

	virtual b8 EndFrame(f32 DeltaTime) = 0;

	RendererBackendType Type;

	MemArena *Arena = nullptr;

	PlatformState *PlatState;

	u64 FrameCount;
};
