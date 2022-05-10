#pragma once

#include <vector>

#include <glm/glm/glm.hpp>

namespace god
{
	struct AssimpModel;
	struct OGLVertex;

	using	ruint = uint32_t;
	using	OGLEnum = uint32_t;

	struct OGLMesh
	{
		static constexpr ruint OGLUNINITIALIZED = static_cast< ruint >( -1 );

		struct OGLVertex
		{
			glm::vec3 m_position;
			glm::vec2 m_uv;
			glm::vec3 m_tangent;
			glm::vec3 m_normal;
			glm::vec4 m_colour;
		};

		using OGLVertices = std::vector<OGLVertex>;
		using OGLIndices = std::vector<ruint>;

		OGLVertices		m_vertices;
		OGLIndices		m_indices;

		void Initialize ();
		void SetData ();
		void Draw ( OGLEnum format ) const;

	private:
		unsigned int m_vao { OGLUNINITIALIZED } , m_vbo { OGLUNINITIALIZED } , m_ebo { OGLUNINITIALIZED };
	};
}