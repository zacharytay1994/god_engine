#include "../pch.h"
#include "Primitives.h"

#include <glad/glad.h>

namespace god
{
	void OGLMesh::Initialize ()
	{
		// create vertex buffer object and bind data
		glCreateBuffers ( 1 , &m_vbo );

		glNamedBufferStorage ( m_vbo , m_vertices.size () * sizeof ( OGLVertex ) , m_vertices.data () , GL_DYNAMIC_STORAGE_BIT );

		// create vertex array object to start recording state
		glCreateVertexArrays ( 1 , &m_vao );
		glVertexArrayVertexBuffer ( m_vao , 0 , m_vbo , 0 , sizeof ( OGLVertex ) );

		// bind m_vao and m_vbo to m_vao for attrib bindings
		glBindVertexArray ( m_vao );
		glBindBuffer ( GL_ARRAY_BUFFER , m_vbo );

		// vertex position
		glEnableVertexAttribArray ( 0 );
		glVertexAttribPointer ( 0 , 3 , GL_FLOAT , GL_FALSE , sizeof ( OGLVertex ) , ( void* ) 0 );

		// vertex normals
		glEnableVertexAttribArray ( 1 );
		glVertexAttribPointer ( 1 , 3 , GL_FLOAT , GL_FALSE , sizeof ( OGLVertex ) , ( void* ) offsetof ( OGLVertex , m_normal ) );

		// create index buffer and bind data
		glCreateBuffers ( 1 , &m_ebo );
		glNamedBufferStorage ( m_ebo , sizeof ( ruint ) * m_indices.size () , m_indices.data () , GL_DYNAMIC_STORAGE_BIT );
		glVertexArrayElementBuffer ( m_vao , m_ebo );

		// unbind vertex array object to stop recording state
		glBindVertexArray ( 0 );
	}

	void OGLMesh::SetData ()
	{
		glBindBuffer ( GL_ARRAY_BUFFER , m_vbo );
		glBufferSubData ( GL_ARRAY_BUFFER , 0 , m_vertices.size () * sizeof ( OGLVertex ) , m_vertices.data () );
		glBindBuffer ( GL_ARRAY_BUFFER , 0 );

		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , m_ebo );
		glBufferSubData ( GL_ELEMENT_ARRAY_BUFFER , 0 , m_indices.size () * sizeof ( ruint ) , m_indices.data () );
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , 0 );
	}

	void OGLMesh::Draw ( OGLEnum format ) const
	{
		// draw mesh
		glBindVertexArray ( m_vao );
		glDrawElements ( format , static_cast< GLsizei >( m_indices.size () ) , GL_UNSIGNED_INT , 0 );
		glBindVertexArray ( 0 );
	}
}