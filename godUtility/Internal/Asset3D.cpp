#include "../pch.h"
#include "Asset3D.h"

namespace god
{
	Asset3D::Asset3D ( std::string const& modelRaw )
		:
		m_model ( modelRaw )
	{
	}

	void Asset3D::Serialize ()
	{
		std::ofstream os ( "test.god3d" , std::ios::binary );
		size_t size = m_model.m_meshes[ 0 ].m_vertices.size ();
		os.write ( ( char* ) &size , sizeof ( size ) );
		os.write ( ( char* ) &m_model.m_meshes[ 0 ].m_vertices[ 0 ] , size * sizeof ( Vertex3D ) );
	}

	void Asset3D::Deserialize ()
	{
		std::ifstream is ( "test.god3d" , std::ios::binary );
		size_t size;
		is.read ( ( char* ) &size , sizeof ( size ) );
		m_model.m_meshes.emplace_back ();
		m_model.m_meshes.back ().m_vertices.resize ( size );
		is.read ( ( char* ) &m_model.m_meshes.back ().m_vertices[ 0 ] , size * sizeof ( Vertex3D ) );
	}
}