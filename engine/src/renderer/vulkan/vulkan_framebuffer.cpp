#include "vulkan_types.h"
#include "vulkan_backend.h"
#include "core/kiwi_mem.h"

void VulkanFramebuffer::Create(VulkanRenderPass *InRenderPass, u32 Width, u32 Height, u32 InAttachmentCount,
							   VkImageView *InAttachments)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	AttachmentCount = InAttachmentCount;
	RenderPass = InRenderPass;

	// NOTE: We copy the attachments array so we don't have to rely on it still
	// existing after this funcion has been called
	// TODO: UNCLEAR ARENA
	Attachments = (VkImageView *)MemSystem::AllocateToUnclearArena(
		sizeof(VkImageView) * AttachmentCount);
	for (u32 Idx = 0; Idx < AttachmentCount; ++Idx)
	{
		Attachments[Idx] = InAttachments[Idx];
	}

	VkFramebufferCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	CreateInfo.renderPass = RenderPass->Handle;
	CreateInfo.attachmentCount = AttachmentCount;
	CreateInfo.pAttachments = Attachments;
	CreateInfo.width = Width;
	CreateInfo.height = Height;
	CreateInfo.layers = 1;

	VK_CHECK(vkCreateFramebuffer(Context->Device.LogicalDevice, &CreateInfo, Context->Allocator, &Handle))
}

void VulkanFramebuffer::Destroy()
{
	VulkanContext *Context = &VulkanRenderer::Context;

	vkDestroyFramebuffer(Context->Device.LogicalDevice, Handle, Context->Allocator);

	if (Attachments)
	{
		// TODO: Free the attachments array once the allocation is no longer inside the unclear arena
		Attachments = nullptr;
	}

	Handle = 0;
	AttachmentCount = 0;
	RenderPass = nullptr;
}