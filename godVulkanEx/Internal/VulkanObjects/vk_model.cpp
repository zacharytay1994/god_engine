#include "vk_model.h"

#include <iostream>

namespace VK_OBJECT
{
	Model::Model (
		Devices const& devices ,
		VkQueue vkGraphicsQueue ,
		VkCommandPool vkCommandPool ,
		Vertices const& vertices ,
		Indices const& indices )
	{
		if ( !m_vertex_buffer.Initialize<Vertices> ( devices , vkGraphicsQueue , vkCommandPool , Buffer::TYPE::VERTEX , vertices ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Model - failed to create vertex buffer!" << std::endl;
			return;
		}
		if ( !m_index_buffer.Initialize<Indices> ( devices , vkGraphicsQueue , vkCommandPool , Buffer::TYPE::INDEX , indices ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Model - failed to create index buffer!" << std::endl;
			return;
		}
		std::cout << "[SUCCESS] VK_OBJECT::Model - Created successfully." << std::endl;

		m_success = true;
	}

	void Model::Bind ( VkCommandBuffer vkCommandBuffer )
	{
		VkBuffer vertex_buffers[] = { m_vertex_buffer.GetBuffer () };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers ( vkCommandBuffer , 0 , 1 , vertex_buffers , offsets );
		vkCmdBindIndexBuffer ( vkCommandBuffer , m_index_buffer.GetBuffer () , 0 , VK_INDEX_TYPE_UINT32 );
	}

	bool Model::Success ()
	{
		return m_success;
	}
}