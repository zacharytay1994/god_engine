#pragma once

#include "vk_devices.h"
#include "vk_vertex.h"
#include "vk_buffer.h"

#include <vulkan/vulkan.h>
#include <glm/glm/glm.hpp>

#include <vector>

namespace VK_OBJECT
{
	using Vertices = std::vector<Vertex>;
	using Indices = std::vector<uint32_t>;

	struct Model
	{
		Buffer m_vertex_buffer;
		Buffer m_index_buffer;

		Model () = default;
		Model (
			Devices const& devices ,
			VkQueue vkGraphicsQueue ,
			VkCommandPool vkCommandPool ,
			Vertices const& vertices ,
			Indices const& indices
		);

		void Bind ( VkCommandBuffer vkCommandBuffer );
		bool Success ();

		struct PushConstant
		{
			glm::mat4 model_matrix_;
			glm::mat4 render_matrix_;
		};

	private:
		bool m_success { false };
	};

	static const Vertices CubeVertices {
		{ {-0.5f, -0.5f,  0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
		{ {0.5f, -0.5f,  0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
		{ {0.5f,  0.5f,  0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
		{ {-0.5f,  0.5f,  0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
		{ {-0.5f, -0.5f, -0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
		{ {-0.5f,  0.5f, -0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
		{ {0.5f,  0.5f, -0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
		{ {0.5f, -0.5f, -0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
		{ {-0.5f,  0.5f, -0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
		{ {-0.5f,  0.5f,  0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
		{ {0.5f,  0.5f,  0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
		{ {0.5f,  0.5f, -0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
		{ {-0.5f, -0.5f, -0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
		{ {0.5f, -0.5f, -0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
		{ {0.5f, -0.5f,  0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
		{ {-0.5f, -0.5f,  0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
		{ {0.5f, -0.5f, -0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
		{ {0.5f,  0.5f, -0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
		{ {0.5f,  0.5f,  0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
		{ {0.5f, -0.5f,  0.5f},   {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
		{ {-0.5f, -0.5f, -0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
		{ {-0.5f, -0.5f,  0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
		{ {-0.5f,  0.5f,  0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
		{ {-0.5f,  0.5f, -0.5f},  {0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f, 1.0f, 0.0f, 1.0f} }
	};

	static const Indices CubeIndices {
		0u,  1u,  2u,      0u,  2u,  3u,    // front
		4u,  5u,  6u,      4u,  6u,  7u,    // back
		8u,  9u,  10u,     8u,  10u, 11u,   // top
		12u, 13u, 14u,     12u, 14u, 15u,   // bottom
		16u, 17u, 18u,     16u, 18u, 19u,   // right
		20u, 21u, 22u,     20u, 22u, 23u    // left
	};
}