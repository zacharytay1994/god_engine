#include "vk_depthbuffer.h"
#include "vk_helper.h"

#include <iostream>
#include <assert.h>

namespace VK_OBJECT
{
	DepthBuffer::DepthBuffer (
		VkPhysicalDevice vkPhysicalDevice ,
		VkDevice vkLogicalDevice ,
		uint32_t width ,
		uint32_t height )
		:
		m_logical_device ( vkLogicalDevice )
	{
		if ( !Create ( vkPhysicalDevice , vkLogicalDevice , width , height ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::DepthBuffer - Failed to create!" << std::endl;
			return;
		}
	}

	DepthBuffer::~DepthBuffer ()
	{
		Destroy ();
	}

	void DepthBuffer::Destroy ()
	{
		assert ( m_logical_device != VK_NULL_HANDLE );
		vkDestroyImageView ( m_logical_device , m_depth_image_view , nullptr );
		vkDestroyImage ( m_logical_device , m_depth_image , nullptr );
		vkFreeMemory ( m_logical_device , m_depth_image_memory , nullptr );
	}

	bool DepthBuffer::Create ( VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , uint32_t width , uint32_t height )
	{
		VkFormat depth_format = VK_HELPER::GET::DepthFormat ( vkPhysicalDevice );

		// create image and memory
		if ( !CreateImage ( vkPhysicalDevice , vkLogicalDevice , width , height , depth_format ,
			VK_IMAGE_TILING_OPTIMAL , VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::DepthBuffer - Failed to create depth image!" << std::endl;
			return false;
		}

		// create image view
		if ( !CreateImageView ( vkLogicalDevice , depth_format , VK_IMAGE_ASPECT_DEPTH_BIT ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::DepthBuffer - Failed to create depth image view!" << std::endl;
			return false;
		}
		return true;
	}

	bool DepthBuffer::CreateImage (
		VkPhysicalDevice vkPhysicalDevice ,
		VkDevice vkLogicalDevice ,
		uint32_t width ,
		uint32_t height ,
		VkFormat depthFormat ,
		VkImageTiling tiling ,
		VkImageUsageFlags usage ,
		VkMemoryPropertyFlags properties )
	{
		VkImageCreateInfo imageInfo {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = depthFormat;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if ( vkCreateImage ( vkLogicalDevice , &imageInfo , nullptr , &m_depth_image ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::DepthBuffer::CreateImage - Failed to create image!" << std::endl;
			return false;
		}

		VkMemoryRequirements memory_requirements;
		vkGetImageMemoryRequirements ( vkLogicalDevice , m_depth_image , &memory_requirements );

		VkMemoryAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memory_requirements.size;
		allocInfo.memoryTypeIndex = VK_HELPER::GET::MemoryType ( vkPhysicalDevice , memory_requirements.memoryTypeBits , properties );

		if ( vkAllocateMemory ( vkLogicalDevice , &allocInfo , nullptr , &m_depth_image_memory ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::DepthBuffer::CreateImage - failed to allocate image memory!" << std::endl;
			return false;
		}

		vkBindImageMemory ( vkLogicalDevice , m_depth_image , m_depth_image_memory , 0 );

		std::cerr << "[SUCCESS] VK_OBJECT::DepthBuffer::CreateImage - Image created successfully!" << std::endl;
		return true;
	}

	bool DepthBuffer::CreateImageView ( VkDevice vkLogicalDevice , VkFormat depthFormat , VkImageAspectFlags aspectFlags )
	{
		VkImageViewCreateInfo viewInfo {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_depth_image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = depthFormat;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if ( vkCreateImageView ( vkLogicalDevice , &viewInfo , nullptr , &m_depth_image_view ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::DepthBuffer::CreateImageView - failed to create texture image view!" << std::endl;
			return false;
		}

		std::cerr << "[SUCCESS] VK_OBJECT::DepthBuffer::CreateImage - Image view created successfully!" << std::endl;
		return true;
	}
}