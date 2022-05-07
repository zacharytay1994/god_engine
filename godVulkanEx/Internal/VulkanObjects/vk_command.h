#pragma once

#include "vk_devices.h"
#include "vk_swapchain.h"
#include "vk_renderpass.h"
#include "vk_framebuffers.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace VK_OBJECT
{
	struct Command
	{
		VkCommandPool					m_command_pool;
		std::vector<VkCommandBuffer>	m_command_buffers;

		Command (
			Devices& devices ,
			Swapchain& swapchain ,
			RenderPass& renderpass ,
			Framebuffers& framebuffers ,
			VkSurfaceKHR surface
		);

		void Destroy ();
		void RecreateCommandBuffers (
			Swapchain& swapchain ,
			RenderPass& renderpass ,
			Framebuffers& framebuffers );

	private:
		Devices& mref_devices;
		Swapchain& mref_swapchain;
		RenderPass& mref_renderpass;
		Framebuffers& mref_framebuffers;

		bool CreateCommandPool ( VkSurfaceKHR surface );
		bool AllocateCommandBuffers ();
	};
}