#pragma once

#include "renderer_frontend.h"
#include "core/logger.h"

RendererBackend *Renderer::Backend = nullptr;

b8 Renderer::Initialize(const char *ApplicationName, PlatformState *PlatState)
{
	// TODO: Vulkan type is hardcoded
	if (RendererBackend::Create(RendererBackendType_Vulkan, PlatState, ApplicationName, &Backend))
	{
		return true;
	}

	LogFatal("Could not create the renderer backend");
	return false;
}

void Renderer::Terminate()
{
	RendererBackend::Destroy(Backend);
}

// TODO: Error
#pragma warning(suppress : 4100)
void Renderer::OnResized(u16 Width, u16 Height)
{
}

b8 Renderer::DrawFrame(RenderPacket *Packet)
{
	// NOTE: if begin frame doesn't succed we cannot proceed
	if (Backend->BeginFrame(Packet->DeltaTime))
	{
		Backend->FrameCount++;

		// TODO: mid-frame operations

		// NOTE: we cannot recover from EndFrame failure
		if (!Backend->EndFrame(Packet->DeltaTime))
		{
			LogFatal("Renderer backend EndFrame() failed. Shutting down.");
			return false;
		}
	}

	return true;
}