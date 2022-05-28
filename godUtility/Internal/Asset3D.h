#pragma once

#include "../godUtility.h"
#include "Miscellaneous.h"
#include "Model3D.h"

#include <string>
#include <vector>
#include <functional>
#include <assert.h>
#include <chrono>
#include <ctime>
#include <numeric>

namespace god
{
	struct Asset3D
	{
		Model3D m_model;

		GODUTILITY_API Asset3D () = default;
		GODUTILITY_API Asset3D ( std::string const& modelRaw );

		void GODUTILITY_API Serialize ( std::string const& path );
		void GODUTILITY_API Deserialize ( std::string const& path );

		GODUTILITY_API static const std::string m_extension;
	};
}

template <>
struct std::hash<god::Asset3D>
{
	std::size_t operator()( god::Asset3D const& asset ) const noexcept
	{
		// calculate average vertex of model
		glm::vec3 average_vertex_position;
		size_t seed = 0;
		for ( auto const& mesh : asset.m_model.m_meshes )
		{
			god::Vertex3D vertex_sum = std::accumulate ( mesh.m_vertices.begin () , mesh.m_vertices.end () , god::Vertex3D () );
			god::Index3D index_sum = std::accumulate ( mesh.m_indices.begin () , mesh.m_indices.end () , god::Index3D () );
			
			god::HashCombine ( seed , vertex_sum.m_position.x + vertex_sum.m_position.y + vertex_sum.m_position.z );
			god::HashCombine ( seed , vertex_sum.m_uv.x + vertex_sum.m_uv.y );
			god::HashCombine ( seed , vertex_sum.m_tangent.x + vertex_sum.m_tangent.y + vertex_sum.m_tangent.z );
			god::HashCombine ( seed , vertex_sum.m_normal.x + vertex_sum.m_normal.y + vertex_sum.m_normal.z );
			god::HashCombine ( seed , vertex_sum.m_colour.x + vertex_sum.m_colour.y + vertex_sum.m_colour.z );
			god::HashCombine ( seed , index_sum );
		}
		return seed;
	}
};