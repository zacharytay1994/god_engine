#include "../pch.h"
#include "Asset3D.h"

namespace god
{
	const std::string Asset3D::m_extension { ".god3d" };

	Asset3D::Asset3D ( std::string const& modelRaw )
		:
		m_model ( modelRaw )
	{
	}

	void Asset3D::Serialize ( std::string const& path )
	{
		std::ofstream os ( path + m_extension , std::ios::binary);

		// serialize meshes size
		size_t mesh_size = m_model.m_meshes.size ();
		os.write ( ( char* ) &mesh_size , sizeof ( mesh_size ) );

		// serialize meshes
		for ( auto const& mesh : m_model.m_meshes )
		{
			// vertices
			size_t vertices_size = mesh.m_vertices.size ();
			os.write ( ( char* ) &vertices_size , sizeof ( vertices_size ) );
			os.write ( ( char* ) mesh.m_vertices.data() , vertices_size * sizeof ( Vertex3D ) );

			// indices
			size_t indices_size = mesh.m_indices.size ();
			os.write ( ( char* ) &indices_size , sizeof ( indices_size ) );
			os.write ( ( char* ) mesh.m_indices.data () , indices_size * sizeof ( Index3D ) );
		}
	}

	void Asset3D::Deserialize ( std::string const& path )
	{
		std::ifstream is ( path + m_extension , std::ios::binary );
		size_t mesh_size;
		size_t vertices_size;
		size_t indices_size;

		// read mesh size
		is.read ( ( char* ) &mesh_size , sizeof ( mesh_size ) );

		for ( size_t i = 0; i < mesh_size; ++i )
		{
			m_model.m_meshes.emplace_back ();

			// read vertices
			is.read ( ( char* ) &vertices_size , sizeof ( vertices_size ) );
			m_model.m_meshes.back ().m_vertices.resize ( vertices_size );
			is.read ( ( char* ) m_model.m_meshes.back ().m_vertices.data() , vertices_size * sizeof ( Vertex3D ) );

			// read indices
			is.read ( ( char* ) &indices_size , sizeof ( indices_size ) );
			m_model.m_meshes.back ().m_indices.resize ( indices_size );
			is.read ( ( char* ) m_model.m_meshes.back ().m_indices.data () , indices_size * sizeof ( Index3D ) );
		}
	}
}