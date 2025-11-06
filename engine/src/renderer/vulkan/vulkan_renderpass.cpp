#include "vulkan_types.h"
#include "vulkan_backend.h"
#include "core/logger.h"

void VulkanRenderPass::Create(u32 InX, u32 InY, u32 InW, u32 InH, f32 InR, f32 InG, f32 InB, f32 InA,
							  f32 InDepth, u32 InStencil)
{
	x = InX;
	y = InY;
	w = InW;
	h = InH;
	r = InR;
	g = InG;
	b = InB;
	a = InA;
	Depth = InDepth;
	Stencil = InStencil;

	VulkanContext *Context = &VulkanRenderer::Context;

	VkSubpassDescription Subpass = {};
	Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	// TODO: Make this configurable
	u32 AttachmentDescriptionCount = 2;
	VkAttachmentDescription AttachmentDescriptions[2];

	// Color Attachment
	VkAttachmentDescription ColorAttachment = {};
	ColorAttachment.format = Context->Swapchain.ImageFormat.format; // TODO: configurable
	ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;	   // Do not expect any particular layout before render pass starts.
	ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Transitioned to after the render pass

	AttachmentDescriptions[0] = ColorAttachment;

	VkAttachmentReference ColorAttachmentReference = {};
	ColorAttachmentReference.attachment = 0; // Attachment description array index
	ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	Subpass.colorAttachmentCount = 1;
	Subpass.pColorAttachments = &ColorAttachmentReference;

	// Depth Attachment
	VkAttachmentDescription DepthAttachment = {};
	DepthAttachment.format = Context->Device.DepthFormat;
	DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	AttachmentDescriptions[1] = DepthAttachment;

	VkAttachmentReference DepthAttachmentReference = {};
	DepthAttachmentReference.attachment = 1;
	DepthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	Subpass.pDepthStencilAttachment = &DepthAttachmentReference;

	// TODO: Allow other attachment types (input, resolve, preserve)
	Subpass.inputAttachmentCount = 0;
	Subpass.pInputAttachments = nullptr;

	Subpass.pResolveAttachments = nullptr;

	Subpass.preserveAttachmentCount = 0;
	Subpass.pPreserveAttachments = nullptr;

	// TODO: Make this configurable
	VkSubpassDependency Dependency;
	Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	Dependency.dstSubpass = 0;
	Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	Dependency.srcAccessMask = 0;
	Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	Dependency.dependencyFlags = 0;

	// Render pass creation
	VkRenderPassCreateInfo RenderPassCreateInfo = {};
	RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassCreateInfo.attachmentCount = AttachmentDescriptionCount;
	RenderPassCreateInfo.pAttachments = AttachmentDescriptions;
	RenderPassCreateInfo.subpassCount = 1;
	RenderPassCreateInfo.pSubpasses = &Subpass;
	RenderPassCreateInfo.dependencyCount = 1;
	RenderPassCreateInfo.pDependencies = &Dependency;

	VK_CHECK(vkCreateRenderPass(Context->Device.LogicalDevice, &RenderPassCreateInfo,
								Context->Allocator, &Handle));
}

void VulkanRenderPass::Destroy()
{
	VulkanContext *Context = &VulkanRenderer::Context;

	if (Handle)
	{
		vkDestroyRenderPass(Context->Device.LogicalDevice, Handle, Context->Allocator);
		Handle = 0;
	}
}

void VulkanRenderPass::Begin(VulkanCommandBuffer *CommandBuffer, VkFramebuffer FrameBuffer)
{
	VkClearValue ClearValues[2] = {};
	ClearValues[0].color.float32[0] = r;
	ClearValues[0].color.float32[1] = g;
	ClearValues[0].color.float32[2] = b;
	ClearValues[0].color.float32[3] = a;
	ClearValues[1].depthStencil.depth = Depth;
	ClearValues[1].depthStencil.stencil = Stencil;

	VkRenderPassBeginInfo BeginInfo = {};
	BeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	BeginInfo.renderPass = Handle;
	BeginInfo.framebuffer = FrameBuffer;
	BeginInfo.renderArea.offset.x = x;
	BeginInfo.renderArea.offset.y = y;
	BeginInfo.renderArea.extent.width = w;
	BeginInfo.renderArea.extent.height = h;
	BeginInfo.clearValueCount = 2;
	BeginInfo.pClearValues = ClearValues;

	vkCmdBeginRenderPass(CommandBuffer->Handle, &BeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	CommandBuffer->State = CBS_InRenderPass;
}

void VulkanRenderPass::End(VulkanCommandBuffer *CommandBuffer)
{
	vkCmdEndRenderPass(CommandBuffer->Handle);
	CommandBuffer->State = CBS_Recording;
}