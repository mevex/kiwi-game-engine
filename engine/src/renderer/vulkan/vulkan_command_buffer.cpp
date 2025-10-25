#include "vulkan_command_buffer.h"

void VulkanCommandBufferAllocate(VulkanContext *Context, VkCommandPool Pool, b8 IsPrimary,
								 VulkanCommandBuffer *OutCommandBuffer)
{
	OutCommandBuffer->State = CBS_NotAllocated;

	VkCommandBufferAllocateInfo AllocateInfo = {};
	AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocateInfo.commandPool = Pool;
	AllocateInfo.level = IsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	AllocateInfo.commandBufferCount = 1;

	VK_CHECK(vkAllocateCommandBuffers(Context->Device.LogicalDevice, &AllocateInfo,
									  &OutCommandBuffer->Handle));

	OutCommandBuffer->State = CBS_Ready;
}

void VulkanCommandBufferFree(VulkanContext *Context, VkCommandPool Pool,
							 VulkanCommandBuffer *CommandBuffer)
{
	vkFreeCommandBuffers(Context->Device.LogicalDevice, Pool, 1, &CommandBuffer->Handle);

	CommandBuffer->Handle = 0;
	CommandBuffer->State = CBS_NotAllocated;
}

void VulkanCommandBufferBegin(VulkanCommandBuffer *CommandBuffer, b8 IsSingleUse, b8 IsRenderPassContinue,
							  b8 IsSimultaneousUse)
{
	VkCommandBufferBeginInfo BeginInfo = {};
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (IsSingleUse)
		BeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	else if (IsRenderPassContinue)
		BeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
	else if (IsSimultaneousUse)
		BeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	VK_CHECK(vkBeginCommandBuffer(CommandBuffer->Handle, &BeginInfo));

	CommandBuffer->State = CBS_Recording;
}

void VulkanCommandBufferEnd(VulkanCommandBuffer *CommandBuffer)
{
	VK_CHECK(vkEndCommandBuffer(CommandBuffer->Handle));
	// TODO: Check that the transition is from a valid state
	CommandBuffer->State = CBS_RecordingEnded;
}

void VulkanCommandBufferUpdateSubmitted(VulkanCommandBuffer *CommandBuffer)
{
	// TODO: Check that the transition is from a valid state
	CommandBuffer->State = CBS_Submitted;
}

void VulkanCommandBufferReset(VulkanCommandBuffer *CommandBuffer)
{
	// TODO: Check that the transition is from a valid state
	CommandBuffer->State = CBS_Ready;
}

void VulkanCommandBufferAllocateAndBeginSingleUse(VulkanContext *Context, VkCommandPool Pool,
												  VulkanCommandBuffer *OutCommandBuffer)
{
	VulkanCommandBufferAllocate(Context, Pool, true, OutCommandBuffer);
	VulkanCommandBufferBegin(OutCommandBuffer, true, false, false);
}

void VulkanCommandBufferEndSingleUse(VulkanContext *Context, VkCommandPool Pool,
									 VulkanCommandBuffer *CommandBuffer, VkQueue Queue)
{
	VulkanCommandBufferEnd(CommandBuffer);

	// Submit the command buffer
	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &CommandBuffer->Handle;

	VK_CHECK(vkQueueSubmit(Queue, 1, &SubmitInfo, 0));

	VK_CHECK(vkQueueWaitIdle(Queue));

	VulkanCommandBufferFree(Context, Pool, CommandBuffer);
}