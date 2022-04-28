#pragma once

#include <vector>

#include <glm/glm/glm.hpp>

namespace god
{
	struct OGLVertex;

	using	ruint	= uint32_t;
	using	OGLEnum = uint32_t;

	struct OGLMesh
	{
		struct OGLVertex
		{
			glm::vec3	m_position;
			glm::vec3	m_normal;
		};

		using OGLVertices = std::vector<OGLVertex>;
		using OGLIndices = std::vector<ruint>;

		OGLVertices		m_vertices;
		OGLIndices		m_indices;

		void Initialize ();
		void SetData ();
		void Draw ( OGLEnum format ) const;

	private:
		unsigned int m_vao , m_vbo , m_ebo;
	};
}