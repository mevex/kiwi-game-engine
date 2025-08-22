#include "renderer_backend.h"
#include "renderer/vulkan/vulkan_backend.h"
#include "core/kiwi_mem.h"
#include <new>

b8 RendererBackend::Create(RendererBackendType Type, PlatformState *PlatState,
						   const char *ApplicationName, RendererBackend **OutRendererBackend)
{
	if (Type == RendererBackendType_Vulkan)
	{
		// NOTE: placement new required to populate the __vfptr
		void *Memory = MemSystem::Allocate(sizeof(VulkanRenderer), MemTag_Renderer);
		*OutRendererBackend = new (Memory) VulkanRenderer(); // TODO: This is done in order for the VTABLE to be populated
	}

	RendererBackend *NewBackend = *OutRendererBackend;
	NewBackend->Type = Type;
	NewBackend->PlatState = PlatState;
	NewBackend->FrameCount = 0;

	if (NewBackend->Initialize(ApplicationName))
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
		MemSystem::Free(RendererBackend, sizeof(VulkanRenderer), MemTag_Renderer);
	}
}