#include "vk_swapchain.h"
#include "vk_helper.h"

#include <algorithm>
#include <iostream>
#include <assert.h>

namespace VK_OBJECT
{
	Swapchain::Swapchain ( uint32_t windowWidth , uint32_t windowHeight ,
		VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , VkSurfaceKHR vkSurface )
		:
		m_window_width ( windowWidth ) ,
		m_window_height ( windowHeight ) ,
		m_logical_device ( vkLogicalDevice )
	{
		if ( !Create ( vkPhysicalDevice , vkLogicalDevice , vkSurface ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Swapchain - Failed to create swap chain." << std::endl;
			return;
		}
	}

	void Swapchain::Destroy ()
	{
		assert ( m_logical_device != VK_NULL_HANDLE );
		for ( size_t i = 0; i < m_image_views.size (); ++i )
		{
			vkDestroyImageView ( m_logical_device , m_image_views[ i ] , nullptr );
		}
		vkDestroySwapchainKHR ( m_logical_device , m_swapchain , nullptr );
	}

	bool Swapchain::Create ( VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , VkSurfaceKHR vkSurface )
	{
		VK_HELPER::SwapChainSupportDetails swapchain_support = VK_HELPER::GET::SwapChainSupportDetails_f ( vkPhysicalDevice , vkSurface );

		// get swap chain formats
		VkSurfaceFormatKHR surface_format = GetSurfaceFormat ( vkPhysicalDevice , vkSurface );
		m_format = surface_format.format;

		// get swap chain present modes
		VkPresentModeKHR present_mode = GetPresentMode ( vkPhysicalDevice , vkSurface );

		// get swap chain extent from capabilities
		m_extent = GetExtent2D ( vkPhysicalDevice , vkSurface );

		uint32_t image_count = swapchain_support.capabilities_.minImageCount + 1;

		if ( swapchain_support.capabilities_.maxImageCount > 0 && image_count > swapchain_support.capabilities_.maxImageCount )
		{
			image_count = swapchain_support.capabilities_.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = vkSurface;
		createInfo.minImageCount = image_count;
		createInfo.imageFormat = surface_format.format;
		createInfo.imageColorSpace = surface_format.colorSpace;
		createInfo.imageExtent = m_extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.presentMode = present_mode;
		// transform of the image in the swap chain, e.g. rotation
		createInfo.preTransform = swapchain_support.capabilities_.currentTransform;
		// how the image blends with other windows
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// if true, pixels blocked by other windows are clipped
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		// queue handling
		VK_HELPER::GET::QueueFamilyIndices indices = VK_HELPER::GET::QueueFamilies ( vkPhysicalDevice , vkSurface );
		uint32_t queueFamilyIndices[] = { indices.graphics_family_.value (), indices.present_family_.value () };
		if ( indices.graphics_family_ != indices.present_family_ )
		{
			// any queue can access the image even from a different queue
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			// only the owning queue can access the swap chain image, more efficient
			// most hardware have the same graphics and presentation queue family,
			// so exclusive if the most used case
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 1;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		if ( vkCreateSwapchainKHR ( vkLogicalDevice , &createInfo , nullptr , &m_swapchain ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Swapchain::Create - Failed to create swap chain." << std::endl;
			return false;
		}

		m_images = GetImages ( vkLogicalDevice );
		m_image_views = CreateImageViews ( vkLogicalDevice );

		m_success = true;
		std::cout << "[SUCCESS] VK_OBJECT::Swapchain::Create - Created successfully." << std::endl;
		return true;
	}

	VkSurfaceFormatKHR Swapchain::GetSurfaceFormat ( VkPhysicalDevice vkPhysicalDevice , VkSurfaceKHR vkSurface )
	{
		std::vector<VkSurfaceFormatKHR> available_formats = VK_HELPER::GET::SwapChainSupportDetails_f ( vkPhysicalDevice , vkSurface ).formats_;
		// if format specified found 
		for ( auto const& available_format : available_formats )
		{
			if ( available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
			{
				return available_format;
			}
		}
		// else return first one
		return available_formats[ 0 ];
	}

	VkPresentModeKHR Swapchain::GetPresentMode ( VkPhysicalDevice vkPhysicalDevice , VkSurfaceKHR vkSurface )
	{
		std::vector<VkPresentModeKHR> available_present_modes = VK_HELPER::GET::SwapChainSupportDetails_f ( vkPhysicalDevice , vkSurface ).present_modes_;
		// if present mode specified found 
		for ( auto const& available_present_mode : available_present_modes )
		{
			if ( available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
			{
				return available_present_mode;
			}
		}
		// else return first in first out
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D Swapchain::GetExtent2D ( VkPhysicalDevice vkPhysicalDevice , VkSurfaceKHR vkSurface )
	{
		VkSurfaceCapabilitiesKHR capabilities = VK_HELPER::GET::SwapChainSupportDetails_f ( vkPhysicalDevice , vkSurface ).capabilities_;
		if ( capabilities.currentExtent.width != UINT32_MAX )
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actual_extent = { static_cast< uint32_t >( m_window_width ), static_cast< uint32_t >( m_window_height ) };

			actual_extent.width = std::clamp ( actual_extent.width , capabilities.minImageExtent.width , capabilities.maxImageExtent.width );
			actual_extent.height = std::clamp ( actual_extent.height , capabilities.minImageExtent.height , capabilities.maxImageExtent.height );

			return actual_extent;
		}
	}

	SwapchainImages Swapchain::GetImages ( VkDevice vkLogicalDevice )
	{
		std::vector<VkImage> images;
		uint32_t image_count { 0 };
		vkGetSwapchainImagesKHR ( vkLogicalDevice , m_swapchain , &image_count , nullptr );
		images.resize ( image_count );
		vkGetSwapchainImagesKHR ( vkLogicalDevice , m_swapchain , &image_count , images.data () );
		return images;
	}

	SwapchainImageViews Swapchain::CreateImageViews ( VkDevice vkLogicalDevice )
	{
		std::vector<VkImageView> image_views;
		image_views.resize ( m_images.size () );

		for ( size_t i = 0; i < m_images.size (); ++i )
		{
			VkImageViewCreateInfo createInfo {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_images[ i ];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if ( vkCreateImageView ( vkLogicalDevice , &createInfo , nullptr , &image_views[ i ] ) != VK_SUCCESS )
			{
				std::cerr << "[ERROR!] VK_OBJECT::Swapchain::GetImageViews - Failed to create image view!" << std::endl;
			}
		}

		return image_views;
	}
}