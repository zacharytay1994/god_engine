#include "vk_renderpass.h"
#include "vk_helper.h"

#include <array>
#include <iostream>
#include <assert.h>

namespace VK_OBJECT
{
	RenderPass::RenderPass ( VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , VkFormat swapchainImageFormat )
		:
		m_logical_device ( vkLogicalDevice )
	{
		if ( !Create ( vkPhysicalDevice , m_logical_device , swapchainImageFormat ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Swapchain - Failed to create swap chain." << std::endl;
			return;
		}
	}

	void RenderPass::Destroy ()
	{
		assert ( m_logical_device != VK_NULL_HANDLE );
		vkDestroyRenderPass ( m_logical_device , m_render_pass , nullptr );
	}

	bool RenderPass::Create ( VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , VkFormat swapchainImageFormat )
	{
		// single color buffer attachment from one of the images from the swap chain
		VkAttachmentDescription colorAttachment {};
		colorAttachment.format = swapchainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// subpasses and attachment references, for postprocessing
		VkAttachmentReference colorAttachmentRef {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// depth attachment
		VkAttachmentDescription depthAttachment {};
		depthAttachment.format = VK_HELPER::GET::DepthFormat ( vkPhysicalDevice );
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		// create render pass
		std::array<VkAttachmentDescription , 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast< uint32_t >( attachments.size () );
		renderPassInfo.pAttachments = attachments.data ();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if ( vkCreateRenderPass ( vkLogicalDevice , &renderPassInfo , nullptr , &m_render_pass ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::RenderPass::Create - Failed to create swap chain." << std::endl;
			return false;
		}

		std::cout << "[SUCCESS] VK_OBJECT::RenderPass::Create - Created successfully." << std::endl;
		m_success = true;
		return true;
	}

	bool RenderPass::Success ()
	{
		return m_success;
	}
}