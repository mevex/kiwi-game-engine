#include "vulkan_framebuffer.h"
#include "core/kiwi_mem.h"

void VulkanFramebufferCreate(VulkanContext *Context, VulkanRenderPass *RenderPass,
							 u32 Width, u32 Height, u32 AttachmentCount, VkImageView *Attachments,
							 VulkanFramebuffer *OutFramebuffer)
{
	// NOTE: We copy the attachments array so we don't have to rely on it still
	// existing after this funcion has been called
	// TODO: UNCLEAR ARENA
	OutFramebuffer->Attachments = (VkImageView *)MemSystem::AllocateToUnclearArena(
		sizeof(VkImageView) * AttachmentCount);
	for (u32 Idx = 0; Idx < AttachmentCount; ++Idx)
	{
		OutFramebuffer->Attachments[Idx] = Attachments[Idx];
	}

	OutFramebuffer->AttachmentCount = AttachmentCount;
	OutFramebuffer->RenderPass = RenderPass;

	VkFramebufferCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	CreateInfo.renderPass = RenderPass->Handle;
	CreateInfo.attachmentCount = AttachmentCount;
	CreateInfo.pAttachments = OutFramebuffer->Attachments;
	CreateInfo.width = Width;
	CreateInfo.height = Height;
	CreateInfo.layers = 1;

	VK_CHECK(vkCreateFramebuffer(Context->Device.LogicalDevice, &CreateInfo, Context->Allocator,
								 &OutFramebuffer->Handle))
}

void VulkanFramebufferDestroy(VulkanContext *Context, VulkanFramebuffer *Framebuffer)
{
	vkDestroyFramebuffer(Context->Device.LogicalDevice, Framebuffer->Handle, Context->Allocator);

	if (Framebuffer->Attachments)
	{
		// TODO: Free the attachments array once the allocation is no longer inside the unclear arena
		Framebuffer->Attachments = nullptr;
	}

	Framebuffer->Handle = 0;
	Framebuffer->AttachmentCount = 0;
	Framebuffer->RenderPass = nullptr;
}