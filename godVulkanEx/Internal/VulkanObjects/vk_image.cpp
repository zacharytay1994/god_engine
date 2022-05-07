#include "vk_image.h"
#include "vk_helper.h"
#include "vk_buffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stdexcept>

#include <iostream>

namespace VK_OBJECT
{
	Image::Image ( std::string const& name , Devices const& devices , Command const& command , VkQueue vkGraphicsQueue )
		:
		m_logical_device { devices.m_logical_device }
	{
		if ( !CreateTextureImage ( name , devices , command , vkGraphicsQueue ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Image - Failed to create image!" << std::endl;
			return;
		}

		if ( !CreateTextureImageView ( devices.m_logical_device ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Image - Failed to create image view!" << std::endl;
			return;
		}

		if ( !CreateTextureSampler ( devices ) )
		{

		}
	}

	Image::~Image ()
	{
		if ( m_logical_device != VK_NULL_HANDLE )
		{
			/*vkDestroySampler ( m_logical_device , m_texture_sampler , nullptr );
			vkDestroyImageView ( m_logical_device , m_texture_image_view , nullptr );
			vkDestroyImage ( m_logical_device , m_texture_image , nullptr );
			vkFreeMemory ( m_logical_device , m_texture_image_memory , nullptr );*/
		}
	}

	void Image::Free ()
	{
		if ( m_logical_device != VK_NULL_HANDLE )
		{
			vkDestroySampler ( m_logical_device , m_texture_sampler , nullptr );
			vkDestroyImageView ( m_logical_device , m_texture_image_view , nullptr );
			vkDestroyImage ( m_logical_device , m_texture_image , nullptr );
			vkFreeMemory ( m_logical_device , m_texture_image_memory , nullptr );
		}
	}

	bool Image::CreateTextureImage ( std::string const& name , Devices const& devices , Command const& command , VkQueue vkGraphicsQueue )
	{
		// get pixel data from image somehow
		// ... to-do
		int tex_width , tex_height , tex_channels;
		stbi_uc* pixels = stbi_load ( name.c_str () , &tex_width , &tex_height , &tex_channels , STBI_rgb_alpha );

		VkDeviceSize image_size = tex_width * tex_height * 4;

		if ( !pixels )
		{
			throw std::runtime_error ( "Failed" );
		}

		VkBuffer		staging_buffer;
		VkDeviceMemory	staging_buffer_memory;

		if ( !Buffer::CreateBuffer ( devices , image_size ,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT ,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ,
			staging_buffer ,
			staging_buffer_memory ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Image::CreateTextureImage - Failed to create buffer for image!" << std::endl;
			return false;
		}

		void* data;
		vkMapMemory ( devices.m_logical_device , staging_buffer_memory , 0 , image_size , 0 , &data );
		memcpy ( data , pixels , static_cast< size_t >( image_size ) );
		vkUnmapMemory ( devices.m_logical_device , staging_buffer_memory );

		stbi_image_free ( pixels );

		CreateImage (
			devices ,
			tex_width ,
			tex_height ,
			VK_FORMAT_R8G8B8A8_SRGB ,
			VK_IMAGE_TILING_OPTIMAL ,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT ,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		TransitionImageLayout ( devices , command , vkGraphicsQueue ,
			VK_FORMAT_R8G8B8A8_SRGB , VK_IMAGE_LAYOUT_UNDEFINED , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

		CopyBufferToImage ( devices , command , vkGraphicsQueue , staging_buffer ,
			static_cast< uint32_t >( tex_width ) , static_cast< uint32_t >( tex_height ) );

		TransitionImageLayout ( devices , command , vkGraphicsQueue ,
			VK_FORMAT_R8G8B8A8_SRGB , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

		vkDestroyBuffer ( devices.m_logical_device , staging_buffer , nullptr );
		vkFreeMemory ( devices.m_logical_device , staging_buffer_memory , nullptr );

		return true;
	}

	bool Image::CreateImage ( Devices const& devices , uint32_t width , uint32_t height , VkFormat format , VkImageTiling tiling , VkImageUsageFlags usage , VkMemoryPropertyFlags properties )
	{
		VkImageCreateInfo image_info {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = width;
		image_info.extent.height = height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.format = format;
		image_info.tiling = tiling;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = usage;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if ( vkCreateImage ( devices.m_logical_device , &image_info , nullptr , &m_texture_image ) != VK_SUCCESS )
		{
			throw std::runtime_error ( "failed to create image!" );
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements ( devices.m_logical_device , m_texture_image , &memRequirements );

		VkMemoryAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VK_HELPER::GET::MemoryType ( devices.m_physical_device , memRequirements.memoryTypeBits , properties );

		if ( vkAllocateMemory ( devices.m_logical_device , &allocInfo , nullptr , &m_texture_image_memory ) != VK_SUCCESS )
		{
			throw std::runtime_error ( "failed to allocate image memory!" );
		}

		vkBindImageMemory ( devices.m_logical_device , m_texture_image , m_texture_image_memory , 0 );

		return true;
	}

	void Image::TransitionImageLayout (
		Devices const& devices ,
		Command const& command ,
		VkQueue vkGraphicsQueue ,
		VkFormat format ,
		VkImageLayout oldLayout ,
		VkImageLayout newLayout )
	{
		( format );

		// begin command
		VkCommandBufferAllocateInfo alloc_info {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = command.m_command_pool;
		alloc_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers ( devices.m_logical_device , &alloc_info , &command_buffer );

		VkCommandBufferBeginInfo begin_info {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer ( command_buffer , &begin_info );

		// end begin command

		VkImageMemoryBarrier barrier {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_texture_image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0; // TODO
		barrier.dstAccessMask = 0; // TODO

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::invalid_argument ( "unsupported layout transition!" );
		}

		vkCmdPipelineBarrier (
			command_buffer ,
			sourceStage /* TODO */ , destinationStage /* TODO */ ,
			0 ,
			0 , nullptr ,
			0 , nullptr ,
			1 , &barrier
		);

		// end command
		vkEndCommandBuffer ( command_buffer );

		VkSubmitInfo submit_info {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit ( vkGraphicsQueue , 1 , &submit_info , VK_NULL_HANDLE );
		vkQueueWaitIdle ( vkGraphicsQueue );

		vkFreeCommandBuffers ( devices.m_logical_device , command.m_command_pool , 1 , &command_buffer );
	}

	void Image::CopyBufferToImage (
		Devices const& devices ,
		Command const& command ,
		VkQueue vkGraphicsQueue ,
		VkBuffer buffer , uint32_t width , uint32_t height )
	{
		// begin command
		VkCommandBufferAllocateInfo alloc_info {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = command.m_command_pool;
		alloc_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers ( devices.m_logical_device , &alloc_info , &command_buffer );

		VkCommandBufferBeginInfo begin_info {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer ( command_buffer , &begin_info );

		// end begin command

		VkBufferImageCopy region {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage (
			command_buffer ,
			buffer ,
			m_texture_image ,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ,
			1 ,
			&region
		);

		// end command
		vkEndCommandBuffer ( command_buffer );

		VkSubmitInfo submit_info {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit ( vkGraphicsQueue , 1 , &submit_info , VK_NULL_HANDLE );
		vkQueueWaitIdle ( vkGraphicsQueue );

		vkFreeCommandBuffers ( devices.m_logical_device , command.m_command_pool , 1 , &command_buffer );
	}

	bool Image::CreateTextureImageView ( VkDevice vkLogicalDevice )
	{
		VkImageViewCreateInfo view_info {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = m_texture_image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = VK_FORMAT_R8G8B8A8_SRGB;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		if ( vkCreateImageView ( vkLogicalDevice , &view_info , nullptr , &m_texture_image_view ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Image::CreateTextureImageView - Failed to create texture image view!" << std::endl;
			return false;
		}
		return true;
	}

	bool Image::CreateTextureSampler ( Devices const& devices )
	{
		VkSamplerCreateInfo sampler_info {};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = VK_FILTER_LINEAR;
		sampler_info.minFilter = VK_FILTER_LINEAR;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.anisotropyEnable = VK_TRUE;
		//sampler_info.maxAnisotropy = 1.0f;

		VkPhysicalDeviceProperties properties {};
		vkGetPhysicalDeviceProperties ( devices.m_physical_device , &properties );

		sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler_info.mipLodBias = 0.0f;
		sampler_info.minLod = 0.0f;
		sampler_info.maxLod = 0.0f;

		if ( vkCreateSampler ( devices.m_logical_device , &sampler_info , nullptr , &m_texture_sampler ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Image::CreateTextureSampler - Failed to create texture sampler!" << std::endl;
			return false;
		}

		return true;
	}
}