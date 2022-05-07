#pragma once

#include "vk_devices.h"
#include "vk_command.h"

#include <vulkan/vulkan.h>
#include <string>

namespace VK_OBJECT
{
	struct Image
	{
		VkImage			m_texture_image { VK_NULL_HANDLE };
		VkDeviceMemory	m_texture_image_memory { VK_NULL_HANDLE };
		VkImageView		m_texture_image_view { VK_NULL_HANDLE };
		VkSampler		m_texture_sampler { VK_NULL_HANDLE };

		Image () = default;
		Image ( std::string const& name , Devices const& devices , Command const& command , VkQueue vkGraphicsQueue );
		~Image ();

		void Free ();

	private:
		VkDevice m_logical_device { VK_NULL_HANDLE };

		bool CreateTextureImage ( std::string const& name , Devices const& devices , Command const& command , VkQueue vkGraphicsQueue );
		bool CreateImage (
			Devices const& devices ,
			uint32_t width ,
			uint32_t height ,
			VkFormat format ,
			VkImageTiling tiling ,
			VkImageUsageFlags usage ,
			VkMemoryPropertyFlags properties );

		void TransitionImageLayout (
			Devices const& devices ,
			Command const& command ,
			VkQueue vkGraphicsQueue ,
			VkFormat format ,
			VkImageLayout oldLayout ,
			VkImageLayout newLayout );

		void CopyBufferToImage (
			Devices const& devices ,
			Command const& command ,
			VkQueue vkGraphicsQueue ,
			VkBuffer buffer , uint32_t width , uint32_t height );

		bool CreateTextureImageView ( VkDevice vkLogicalDevice );

		bool CreateTextureSampler ( Devices const& devices );
	};
}