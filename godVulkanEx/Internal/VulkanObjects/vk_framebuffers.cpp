#include "vk_framebuffers.h"

#include <array>
#include <iostream>

namespace VK_OBJECT
{
	Framebuffers::Framebuffers ( VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , Swapchain swapchain , VkRenderPass renderpass )
		:
		m_logical_device ( vkLogicalDevice ) ,
		m_depth_buffer ( vkPhysicalDevice , vkLogicalDevice , swapchain.m_extent.width , swapchain.m_extent.height )
	{
		m_framebuffers.resize ( swapchain.m_image_views.size () );

		// iterate image views and create framebuffers from them
		for ( size_t i = 0; i < swapchain.m_image_views.size (); ++i )
		{
			/*VkImageView attachments[] = {
				swapChainData.image_views_[ i ]
			};*/

			std::array<VkImageView , 2> attachments = {
				swapchain.m_image_views[ i ],
				m_depth_buffer.m_depth_image_view
			};

			VkFramebufferCreateInfo framebufferInfo {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderpass;
			framebufferInfo.attachmentCount = static_cast< uint32_t >( attachments.size () );
			framebufferInfo.pAttachments = attachments.data ();
			framebufferInfo.width = swapchain.m_extent.width;
			framebufferInfo.height = swapchain.m_extent.height;
			framebufferInfo.layers = 1;

			if ( vkCreateFramebuffer ( vkLogicalDevice , &framebufferInfo , nullptr , &m_framebuffers[ i ] ) != VK_SUCCESS )
			{
				std::cerr << "[ERROR!] VK_OBJECT::Framebuffers - Failed to create framebuffer " << i << "." << std::endl;
				return;
			}
		}

		std::cout << "[SUCCESS] VK_OBJECT::Framebuffers - framebuffers created successfully!" << std::endl;
	}

	Framebuffers::~Framebuffers ()
	{
		for ( size_t i = 0; i < m_framebuffers.size (); i++ )
		{
			vkDestroyFramebuffer ( m_logical_device , m_framebuffers[ i ] , nullptr );
		}
	}

	size_t Framebuffers::Size ()
	{
		return m_framebuffers.size ();
	}

	void Framebuffers::Destroy ()
	{
		m_depth_buffer.Destroy ();
		for ( size_t i = 0; i < m_framebuffers.size (); i++ )
		{
			vkDestroyFramebuffer ( m_logical_device , m_framebuffers[ i ] , nullptr );
		}
	}

	void Framebuffers::Create ( VkPhysicalDevice vkPhysicalDevice , VkDevice vkLogicalDevice , Swapchain swapchain , VkRenderPass renderpass )
	{
		m_depth_buffer.Create ( vkPhysicalDevice , vkLogicalDevice , swapchain.m_extent.width , swapchain.m_extent.height );

		m_framebuffers.resize ( swapchain.m_image_views.size () );

		// iterate image views and create framebuffers from them
		for ( size_t i = 0; i < swapchain.m_image_views.size (); ++i )
		{
			/*VkImageView attachments[] = {
				swapChainData.image_views_[ i ]
			};*/

			std::array<VkImageView , 2> attachments = {
				swapchain.m_image_views[ i ],
				m_depth_buffer.m_depth_image_view
			};

			VkFramebufferCreateInfo framebufferInfo {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderpass;
			framebufferInfo.attachmentCount = static_cast< uint32_t >( attachments.size () );
			framebufferInfo.pAttachments = attachments.data ();
			framebufferInfo.width = swapchain.m_extent.width;
			framebufferInfo.height = swapchain.m_extent.height;
			framebufferInfo.layers = 1;

			if ( vkCreateFramebuffer ( vkLogicalDevice , &framebufferInfo , nullptr , &m_framebuffers[ i ] ) != VK_SUCCESS )
			{
				std::cerr << "[ERROR!] VK_OBJECT::Framebuffers - Failed to create framebuffer " << i << "." << std::endl;
				return;
			}
		}

		std::cout << "[SUCCESS] VK_OBJECT::Framebuffers - framebuffers created successfully!" << std::endl;
	}
}