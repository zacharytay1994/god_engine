#include "vk_buffer.h"
#include "vk_helper.h"

namespace VK_OBJECT
{
	VkBuffer Buffer::GetBuffer ()
	{
		return m_buffer;
	}
	VkDeviceMemory Buffer::GetMemory ()
	{
		return m_buffer_device_memory;
	}
	uint32_t Buffer::GetSize ()
	{
		return m_size;
	}
	bool Buffer::CreateBuffer ( Devices const& devices , VkDeviceSize size , VkBufferUsageFlags usage , VkMemoryPropertyFlags properties , VkBuffer& buffer , VkDeviceMemory& bufferMemory )
	{
			VkBufferCreateInfo bufferInfo {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if ( vkCreateBuffer ( devices.m_logical_device , &bufferInfo , nullptr , &buffer ) != VK_SUCCESS )
			{
				std::cerr << "[ERROR!] VK_OBJECT::Buffer::CreateBuffer - failed to create buffer!" << std::endl;
				return false;
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements ( devices.m_logical_device , buffer , &memRequirements );

			VkMemoryAllocateInfo allocInfo {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = VK_HELPER::GET::MemoryType ( devices.m_physical_device , memRequirements.memoryTypeBits , properties );

			if ( vkAllocateMemory ( devices.m_logical_device , &allocInfo , nullptr , &bufferMemory ) != VK_SUCCESS )
			{
				std::cerr << "[ERROR!] VK_OBJECT::Buffer::CreateBuffer - failed to allocate memory!" << std::endl;
				return false;
			}

			vkBindBufferMemory ( devices.m_logical_device , buffer , bufferMemory , 0 );

			return true;
	}

	void Buffer::CopyBuffer ( VkDevice logicalDevice , VkQueue graphicsQueue , VkCommandPool commandPool , VkBuffer srcBuffer , VkBuffer dstBuffer , VkDeviceSize size )
	{
		VkCommandBufferAllocateInfo alloc_info {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = commandPool;
		alloc_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers ( logicalDevice , &alloc_info , &command_buffer );

		VkCommandBufferBeginInfo begin_info {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer ( command_buffer , &begin_info );

		VkBufferCopy copy_region {};
		copy_region.srcOffset = 0; // Optional
		copy_region.dstOffset = 0; // Optional
		copy_region.size = size;
		vkCmdCopyBuffer ( command_buffer , srcBuffer , dstBuffer , 1 , &copy_region );

		vkEndCommandBuffer ( command_buffer );

		VkSubmitInfo submit_info {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit ( graphicsQueue , 1 , &submit_info , VK_NULL_HANDLE );
		vkQueueWaitIdle ( graphicsQueue );

		vkFreeCommandBuffers ( logicalDevice , commandPool , 1 , &command_buffer );
	}
}