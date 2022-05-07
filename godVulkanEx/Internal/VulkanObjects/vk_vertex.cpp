#include "vk_vertex.h"

namespace VK_OBJECT
{
	VkVertexInputBindingDescription Vertex::GetBindingDescription ()
	{
		VkVertexInputBindingDescription binding_description {};
		binding_description.binding = 0;
		binding_description.stride = sizeof ( Vertex );
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return binding_description;
	}

	Vertex::AttributeDescriptions Vertex::GetAttributeDescriptions ()
	{
		AttributeDescriptions attribute_descriptions {};

		// position
		attribute_descriptions[ 0 ].binding = 0;
		attribute_descriptions[ 0 ].location = 0;
		attribute_descriptions[ 0 ].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[ 0 ].offset = offsetof ( Vertex , m_position );

		// uv
		attribute_descriptions[ 1 ].binding = 0;
		attribute_descriptions[ 1 ].location = 1;
		attribute_descriptions[ 1 ].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[ 1 ].offset = offsetof ( Vertex , m_uv );

		// tangent
		attribute_descriptions[ 2 ].binding = 0;
		attribute_descriptions[ 2 ].location = 2;
		attribute_descriptions[ 2 ].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[ 2 ].offset = offsetof ( Vertex , m_tangent );

		// normal
		attribute_descriptions[ 3 ].binding = 0;
		attribute_descriptions[ 3 ].location = 3;
		attribute_descriptions[ 3 ].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[ 3 ].offset = offsetof ( Vertex , m_normal );

		// colour
		attribute_descriptions[ 4 ].binding = 0;
		attribute_descriptions[ 4 ].location = 4;
		attribute_descriptions[ 4 ].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[ 4 ].offset = offsetof ( Vertex , m_colour );

		return attribute_descriptions;
	}

	Vertex::InputStateCreateInfo Vertex::GetInputStateCreateInfoData ()
	{
		InputStateCreateInfo state_create_info {};

		state_create_info.m_binding_descriptions = GetBindingDescription ();
		state_create_info.m_attribute_descriptions = GetAttributeDescriptions ();

		return state_create_info;
	}

	VkPipelineVertexInputStateCreateInfo Vertex::InputStateCreateInfo::Get ()
	{
		VkPipelineVertexInputStateCreateInfo out {};

		out.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		out.vertexBindingDescriptionCount = 1;
		out.pVertexBindingDescriptions = &m_binding_descriptions;
		out.vertexAttributeDescriptionCount = static_cast< uint32_t >( m_attribute_descriptions.size () );
		out.pVertexAttributeDescriptions = m_attribute_descriptions.data ();

		return out;
	}
}