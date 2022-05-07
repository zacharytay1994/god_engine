#pragma once

#include <glm/glm/glm.hpp>

#include <vulkan/vulkan.h>
#include <array>

namespace VK_OBJECT
{
	struct Vertex
	{
		using AttributeDescriptions = std::array<VkVertexInputAttributeDescription , 5>;

		glm::vec3 m_position;
		glm::vec2 m_uv;
		glm::vec3 m_tangent;
		glm::vec3 m_normal;
		glm::vec4 m_colour;

		static VkVertexInputBindingDescription GetBindingDescription ();

		static AttributeDescriptions GetAttributeDescriptions ();

		struct InputStateCreateInfo
		{
			friend struct Vertex;
			VkPipelineVertexInputStateCreateInfo Get ();
		private:
			VkVertexInputBindingDescription			m_binding_descriptions;
			AttributeDescriptions					m_attribute_descriptions;
		};
		static InputStateCreateInfo GetInputStateCreateInfoData ();
	};
}