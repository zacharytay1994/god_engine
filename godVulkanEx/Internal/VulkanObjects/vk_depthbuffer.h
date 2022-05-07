#pragma once

#include "vk_swapchain.h"

#include <vulkan/vulkan.h>

namespace VK_OBJECT
{
	struct DepthBuffer
	{
		VkImage			m_depth_image { VK_NULL_HANDLE };
		VkImageView		m_depth_image_view { VK_NULL_HANDLE };
		VkDeviceMemory	m_depth_image_memory { VK_NULL_HANDLE };

		DepthBuffer (
			VkPhysicalDevice vkPhysicalDevice ,
			VkDevice vkLogicalDevice ,
			uint32_t width,
			uint32_t height );

		~DepthBuffer ();
		void Destroy ();
		bool Create (
			VkPhysicalDevice vkPhysicalDevice ,
			VkDevice vkLogicalDevice ,
			uint32_t width ,
			uint32_t height );

	private:
		VkDevice m_logical_device { VK_NULL_HANDLE };

		bool CreateImage (
			VkPhysicalDevice vkPhysicalDevice ,
			VkDevice vkLogicalDevice ,
			uint32_t width ,
			uint32_t height ,
			VkFormat depthFormat ,
			VkImageTiling tiling ,
			VkImageUsageFlags usage ,
			VkMemoryPropertyFlags properties );

		bool CreateImageView (
			VkDevice vkLogicalDevice ,
			VkFormat depthFormat ,
			VkImageAspectFlags aspectFlags );
	};
}