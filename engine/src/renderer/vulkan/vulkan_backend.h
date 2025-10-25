#pragma once

#include "renderer/renderer_backend.h"
#include "vulkan_types.h"

class VulkanRenderer : public RendererBackend
{
public:
	b8 Initialize(const char *ApplicationName) override;

	void Terminate() override;

	void Resized() override;

	b8 BeginFrame(f32 DeltaTime) override;

	b8 EndFrame(f32 DeltaTime) override;

	static VulkanContext Context;

private:
	void CreateCommandBuffers();

	void DestroyCommandBuffers();
};