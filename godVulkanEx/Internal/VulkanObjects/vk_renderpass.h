#pragma once

#include <vulkan/vulkan.h>

namespace VK_OBJECT
{
	struct RenderPass
	{
		VkRenderPass m_render_pass;

		RenderPass ( VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , VkFormat swapchainImageFormat );

		void Destroy ();
		bool Create ( VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , VkFormat swapchainImageFormat );

		bool Success ();

	private:
		bool m_success { false };
		VkDevice m_logical_device { VK_NULL_HANDLE };
	};
}