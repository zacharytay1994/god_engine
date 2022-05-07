#include "vk_command.h"
#include "vk_helper.h"

#include <iostream>

namespace VK_OBJECT
{
	Command::Command (
		Devices& devices ,
		Swapchain& swapchain ,
		RenderPass& renderpass ,
		Framebuffers& framebuffers ,
		VkSurfaceKHR surface
	)
		:
		mref_devices ( devices ) ,
		mref_swapchain ( swapchain ) ,
		mref_renderpass ( renderpass ) ,
		mref_framebuffers ( framebuffers )
	{
		// create command pool
		if ( !CreateCommandPool ( surface ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Command - Failed to create command pool!" << std::endl;
			return;
		}

		// allocate command buffers
		if ( !AllocateCommandBuffers () )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Command - Failed to allocate command buffers!" << std::endl;
			return;
		}
	}

	void Command::Destroy ()
	{
		vkFreeCommandBuffers ( mref_devices.m_logical_device , m_command_pool , static_cast< uint32_t >( m_command_buffers.size () ) , m_command_buffers.data () );
	}

	void Command::RecreateCommandBuffers ( Swapchain& swapchain , RenderPass& renderpass , Framebuffers& framebuffers )
	{
		mref_swapchain = swapchain;
		mref_renderpass = renderpass;
		mref_framebuffers = framebuffers;

		// allocate command buffers
		if ( !AllocateCommandBuffers () )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Command - Failed to allocate command buffers!" << std::endl;
			return;
		}
	}

	bool Command::CreateCommandPool ( VkSurfaceKHR surface )
	{
		VK_HELPER::GET::QueueFamilyIndices queueFamilyIndices = VK_HELPER::GET::QueueFamilies ( mref_devices.m_physical_device , surface );

		VkCommandPoolCreateInfo pool_info {};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.queueFamilyIndex = queueFamilyIndices.graphics_family_.value ();
		pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if ( vkCreateCommandPool ( mref_devices.m_logical_device , &pool_info , nullptr , &m_command_pool ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Command::CreateCommandPool - Failed to create command pool!" << std::endl;
			return false;
		}
		std::cerr << "[SUCCESS] VK_OBJECT::Command::CreateCommandPool - Created successfully." << std::endl;
		return true;
	}

	bool Command::AllocateCommandBuffers ()
	{
		m_command_buffers.resize ( mref_framebuffers.Size () );

		VkCommandBufferAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_command_pool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = ( uint32_t ) m_command_buffers.size ();

		if ( vkAllocateCommandBuffers ( mref_devices.m_logical_device , &allocInfo , m_command_buffers.data () ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Command::AllocateCommandBuffers - Failed to allocate command buffers!" << std::endl;
			return false;
		}
		std::cerr << "[SUCCESS] VK_OBJECT::Command::AllocateCommandBuffers - Created successfully." << std::endl;
		return true;
	}
}