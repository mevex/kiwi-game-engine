#include "vulkan_command_buffer.h"
#include "vulkan_types.h"
#include "vulkan_backend.h"

void VulkanCommandBuffer::Allocate(VkCommandPool Pool, b8 IsPrimary)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	State = CBS_NotAllocated;

	VkCommandBufferAllocateInfo AllocateInfo = {};
	AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocateInfo.commandPool = Pool;
	AllocateInfo.level = IsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	AllocateInfo.commandBufferCount = 1;

	VK_CHECK(vkAllocateCommandBuffers(Context->Device.LogicalDevice, &AllocateInfo, &Handle));

	State = CBS_Ready;
}

void VulkanCommandBuffer::Free(VkCommandPool Pool)
{
	VulkanContext *Context = &VulkanRenderer::Context;

	vkFreeCommandBuffers(Context->Device.LogicalDevice, Pool, 1, &Handle);

	Handle = 0;
	State = CBS_NotAllocated;
}

void VulkanCommandBuffer::Begin(b8 IsSingleUse, b8 IsRenderPassContinue, b8 IsSimultaneousUse)
{
	VkCommandBufferBeginInfo BeginInfo = {};
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (IsSingleUse)
		BeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	else if (IsRenderPassContinue)
		BeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
	else if (IsSimultaneousUse)
		BeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	VK_CHECK(vkBeginCommandBuffer(Handle, &BeginInfo));

	State = CBS_Recording;
}

void VulkanCommandBuffer::End()
{
	VK_CHECK(vkEndCommandBuffer(Handle));
	// TODO: Check that the transition is from a valid state
	State = CBS_RecordingEnded;
}

void VulkanCommandBuffer::UpdateSubmitted()
{
	// TODO: Check that the transition is from a valid state
	State = CBS_Submitted;
}

void VulkanCommandBuffer::Reset()
{
	// TODO: Check that the transition is from a valid state
	State = CBS_Ready;
}

void VulkanCommandBuffer::AllocateAndBeginSingleUse(VkCommandPool Pool)
{
	Allocate(Pool, true);
	Begin(true, false, false);
}

void VulkanCommandBuffer::EndSingleUse(VkCommandPool Pool, VkQueue Queue)
{
	End();

	// Submit the command buffer
	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &Handle;

	VK_CHECK(vkQueueSubmit(Queue, 1, &SubmitInfo, 0));

	VK_CHECK(vkQueueWaitIdle(Queue));

	Free(Pool);
}