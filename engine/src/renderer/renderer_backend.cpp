#include "renderer_backend.h"
#include "renderer/vulkan/vulkan_backend.h"
#include "core/kiwi_mem.h"
#include <new>

b8 RendererBackend::Create(RendererBackendType Type, PlatformState *PlatState, const char *ApplicationName,
						   u32 Width, u32 Height, RendererBackend **OutRendererBackend)
{
	if (Type == RendererBackendType_Vulkan)
	{
		// NOTE: placement new required to populate the __vfptr
		void *Memory = MemSystem::GetArena(MemTag_Renderer)->Push(sizeof(VulkanRenderer));
		*OutRendererBackend = new (Memory) VulkanRenderer(); // TODO: This is done in order for the VTABLE to be populated
	}
	else
	{
		// TODO: Direct3D one day? Who knows...
		LogFatal("Renderer backend type not supported");
		KDebugBreak();
	}

	RendererBackend *NewBackend = *OutRendererBackend;
	NewBackend->Arena = MemSystem::GetArena(MemTag_Renderer);
	NewBackend->Type = Type;
	NewBackend->PlatState = PlatState;
	NewBackend->FrameCount = 0;

	if (NewBackend->Initialize(ApplicationName, Width, Height))
	{
		return true;
	}

	return false;
}

void RendererBackend::Destroy(RendererBackend *RendererBackend)
{
	if (RendererBackend->Type == RendererBackendType_Vulkan)
	{
		RendererBackend->Terminate();
		RendererBackend->Arena->Clear();
	}
}