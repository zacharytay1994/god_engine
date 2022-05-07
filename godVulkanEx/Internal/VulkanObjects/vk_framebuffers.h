#pragma once

#include "vk_swapchain.h"
#include "vk_depthbuffer.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace VK_OBJECT
{
	struct Framebuffers
	{
		DepthBuffer m_depth_buffer;
		std::vector<VkFramebuffer> m_framebuffers;

		Framebuffers ( VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , Swapchain swapchain , VkRenderPass renderpass );
		~Framebuffers ();

		size_t Size ();

		void Destroy ();
		void Create ( VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , Swapchain swapchain , VkRenderPass renderpass );

	private:
		VkDevice m_logical_device { VK_NULL_HANDLE };
	};
}