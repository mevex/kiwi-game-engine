#pragma once

#include "renderer/renderer_types.h"
#include "renderer/renderer_backend.h"

struct PlatformState;

class Renderer
{
public:
	static b8 Initialize(const char *ApplicationName, PlatformState *PlatState);

	static void Terminate();

	static void OnResized(u16 Width, u16 Height);

	static b8 DrawFrame(RenderPacket *Packet);

	static RendererBackend *Backend;
};