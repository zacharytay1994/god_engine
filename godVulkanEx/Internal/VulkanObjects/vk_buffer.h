#pragma once

#include "vk_devices.h"

#include <vulkan/vulkan.h>
#include <iostream>

namespace VK_OBJECT
{
	struct Buffer
	{
		enum class TYPE
		{
			VERTEX ,
			INDEX
		};

		Buffer () = default;

		template <typename BUFFER>
		bool Initialize ( Devices const& devices , VkQueue vkGraphicsQueue , VkCommandPool vkCommandPool , TYPE type , BUFFER const& bufferData )
		{
			VkBufferUsageFlagBits buffer_type_bit { static_cast< VkBufferUsageFlagBits >( 0 ) };
			if ( type == TYPE::VERTEX )
			{
				buffer_type_bit = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			}
			else if ( type == TYPE::INDEX )
			{
				buffer_type_bit = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			}

			m_size = static_cast< uint32_t >( bufferData.size () );

			VkDeviceSize bufferSize = sizeof ( bufferData[ 0 ] ) * bufferData.size ();

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			if ( !CreateBuffer ( devices , bufferSize , VK_BUFFER_USAGE_TRANSFER_SRC_BIT , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT , stagingBuffer , stagingBufferMemory ) )
			{
				std::cerr << "[ERROR!] VK_OBJECT::Buffer::Initialize - failed to create buffer!" << std::endl;
				return false;
			}

			void* data;
			vkMapMemory ( devices.m_logical_device , stagingBufferMemory , 0 , bufferSize , 0 , &data );
			memcpy ( data , bufferData.data () , ( size_t ) bufferSize );
			vkUnmapMemory ( devices.m_logical_device , stagingBufferMemory );

			if ( !CreateBuffer ( devices , bufferSize , VK_BUFFER_USAGE_TRANSFER_DST_BIT | buffer_type_bit , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT , m_buffer , m_buffer_device_memory ) )
			{
				std::cerr << "[ERROR!] VK_OBJECT::Buffer::Initialize - failed to create buffer!" << std::endl;
				return false;
			}

			CopyBuffer ( devices.m_logical_device , vkGraphicsQueue , vkCommandPool , stagingBuffer , m_buffer , bufferSize );

			vkDestroyBuffer ( devices.m_logical_device , stagingBuffer , nullptr );
			vkFreeMemory ( devices.m_logical_device , stagingBufferMemory , nullptr );

			return true;
		}

		VkBuffer		GetBuffer ();
		VkDeviceMemory	GetMemory ();
		uint32_t		GetSize ();

		static bool CreateBuffer ( Devices const& devices , VkDeviceSize size , VkBufferUsageFlags usage , VkMemoryPropertyFlags properties , VkBuffer& buffer , VkDeviceMemory& bufferMemory );

	private:
		// buffer
		VkBuffer		m_buffer { VK_NULL_HANDLE };
		VkDeviceMemory	m_buffer_device_memory { VK_NULL_HANDLE };
		uint32_t		m_size { 0 };

		void CopyBuffer ( VkDevice logicalDevice , VkQueue graphicsQueue , VkCommandPool commandPool , VkBuffer srcBuffer , VkBuffer dstBuffer , VkDeviceSize size );
	};
}