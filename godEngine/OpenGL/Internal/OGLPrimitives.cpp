#include "../../pch.h"
#include "OGLPrimitives.h"

#pragma comment (lib, "opengl32.lib")

#include <glad/glad.h>

namespace god
{
	void OGLMesh::Initialize ()
	{
		// create vertex buffer object and bind data
		glCreateBuffers ( 1 , &m_vbo );
		glCreateBuffers ( 1 , &m_instanced_vbo );

		glNamedBufferStorage ( m_vbo , m_vertices.size () * sizeof ( OGLVertex ) , m_vertices.data () , GL_DYNAMIC_STORAGE_BIT );
		m_transforms.resize ( 1000 );
		glNamedBufferStorage ( m_instanced_vbo , m_transforms.size () * sizeof ( glm::mat4 ) , m_transforms.data () , GL_DYNAMIC_STORAGE_BIT );

		// create vertex array object to start recording state
		glCreateVertexArrays ( 1 , &m_vao );
		//glVertexArrayVertexBuffer ( m_vao , 0 , m_vbo , 0 , sizeof ( OGLVertex ) );

		// bind m_vao and m_vbo to m_vao for attrib bindings
		glBindVertexArray ( m_vao );

		// bind normal vertex attributes
		glBindBuffer ( GL_ARRAY_BUFFER , m_vbo );

		// VERTEX FORMAT 
		// vertex position
		glEnableVertexAttribArray ( 0 );
		glVertexAttribPointer ( 0 , 3 , GL_FLOAT , GL_FALSE , sizeof ( OGLVertex ) , ( void* ) 0 );

		// vertex uv
		glEnableVertexAttribArray ( 1 );
		glVertexAttribPointer ( 1 , 2 , GL_FLOAT , GL_FALSE , sizeof ( OGLVertex ) , ( void* ) offsetof ( OGLVertex , m_uv ) );

		// vertex tangent
		glEnableVertexAttribArray ( 2 );
		glVertexAttribPointer ( 2 , 3 , GL_FLOAT , GL_FALSE , sizeof ( OGLVertex ) , ( void* ) offsetof ( OGLVertex , m_tangent ) );

		// vertex normals
		glEnableVertexAttribArray ( 3 );
		glVertexAttribPointer ( 3 , 3 , GL_FLOAT , GL_FALSE , sizeof ( OGLVertex ) , ( void* ) offsetof ( OGLVertex , m_normal ) );

		// vertex colour
		glEnableVertexAttribArray ( 4 );
		glVertexAttribPointer ( 4 , 4 , GL_FLOAT , GL_FALSE , sizeof ( OGLVertex ) , ( void* ) offsetof ( OGLVertex , m_colour ) );

		// bind transform attributes
		//glVertexArrayVertexBuffer ( m_vao , 1 , m_instanced_vbo , 0 , sizeof ( glm::mat4 ) );
		glBindBuffer ( GL_ARRAY_BUFFER , m_instanced_vbo );
		GLsizei vec4Size = sizeof ( glm::vec4 );
		glEnableVertexAttribArray ( 5 );
		glVertexAttribPointer ( 5 , 4 , GL_FLOAT , GL_FALSE , 4 * vec4Size , ( void* ) 0 );
		glEnableVertexAttribArray ( 6 );
		glVertexAttribPointer ( 6 , 4 , GL_FLOAT , GL_FALSE , 4 * vec4Size , ( void* ) static_cast< long long >( 1 * vec4Size ) );
		glEnableVertexAttribArray ( 7 );
		glVertexAttribPointer ( 7 , 4 , GL_FLOAT , GL_FALSE , 4 * vec4Size , ( void* ) static_cast< long long >( 2 * vec4Size ) );
		glEnableVertexAttribArray ( 8 );
		glVertexAttribPointer ( 8 , 4 , GL_FLOAT , GL_FALSE , 4 * vec4Size , ( void* ) static_cast< long long >( 3 * vec4Size ) );
		glVertexAttribDivisor ( 5 , 1 );
		glVertexAttribDivisor ( 6 , 1 );
		glVertexAttribDivisor ( 7 , 1 );
		glVertexAttribDivisor ( 8 , 1 );

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

	void OGLMesh::SetTransformData ( std::vector<glm::mat4>const& transforms )
	{
		//m_transform_size = transforms.size ();
		m_transform_size = static_cast< uint32_t >( transforms.size () );
		glBindBuffer ( GL_ARRAY_BUFFER , m_instanced_vbo );
		glBufferSubData ( GL_ARRAY_BUFFER , 0 , transforms.size () * sizeof ( glm::mat4 ) , transforms.data () );
		glBindBuffer ( GL_ARRAY_BUFFER , 0 );
	}

	void OGLMesh::DrawInstanced ( OGLEnum format ) const
	{
		glBindVertexArray ( m_vao );
		glDrawElementsInstanced ( format , static_cast< GLsizei >( m_indices.size () ) , GL_UNSIGNED_INT , 0 , m_transform_size );
		glBindVertexArray ( 0 );
	}
}