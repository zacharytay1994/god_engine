#include "../../pch.h"
#include "CubeMap.h"

#include <glad/glad.h>
#pragma comment (lib, "opengl32.lib")

#include <godUtility/Internal/TextureData.h>

#include <iostream>
#include "ShadowMap.h"

namespace god
{
	float skyboxVertices[] =
	{
		//   Coordinates
		// 1.0f,  1.0f,  -1.0f,//1        2--------1
		//-1.0f,  1.0f,  -1.0f,//2       /|       /|
		//-1.0f,  1.0f,   1.0f,//3      3--------4 |
		// 1.0f,  1.0f,   1.0f,//4      | |      | |
		// 1.0f, -1.0f,  -1.0f,//5      | 6------|-5
		//-1.0f, -1.0f,  -1.0f,//6      |/       |/
		//-1.0f, -1.0f,   1.0f,//7      7--------8
		// 1.0f, -1.0f,   1.0f //8

		 //   Coordinates
		-1.0f, -1.0f,  1.0f,// 0       7--------6
		 1.0f, -1.0f,  1.0f,// 1     /|       /|
		 1.0f, -1.0f, -1.0f,// 2     4--------5 |
		-1.0f, -1.0f, -1.0f,// 3    | |      | |
		-1.0f,  1.0f,  1.0f,// 4    | 3------|-2
		 1.0f,  1.0f,  1.0f,// 5    |/       |/
		 1.0f,  1.0f, -1.0f,// 6    0--------1
		-1.0f,  1.0f, -1.0f // 7
	};

	unsigned int skyboxIndices[] =
	{
		7,3,2,
		2,6,7,

		0,3,7,
		7,4,0,

		2,1,5,
		5,6,2,

		0,4,5,
		5,1,0,

		7,6,5,
		5,4,7,

		3,0,2,
		2,0,1
	};

	void OGLCubeMap::Initialize( )
	{
		// Create VAO, VBO, and EBO for the skybox
		glGenVertexArrays( 1, &m_skybox_vao );
		glGenBuffers( 1, &m_skybox_vbo );
		glGenBuffers( 1, &m_skybox_ebo );
		glBindVertexArray( m_skybox_vao );
		glBindBuffer( GL_ARRAY_BUFFER, m_skybox_vbo );
		glBufferData( GL_ARRAY_BUFFER, sizeof( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_skybox_ebo );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( skyboxIndices ), &skyboxIndices, GL_STATIC_DRAW );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), ( void* ) 0 );
		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindVertexArray( 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	}

	void OGLCubeMap::CubeTexture( std::string facesCubemap[6] )
	{

		// Creates the cubemap texture object
		glGenTextures( 1, &m_cubemap_texture );
		glBindTexture( GL_TEXTURE_CUBE_MAP, m_cubemap_texture );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		// These are very important to prevent seams
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
		// This might help with seams on some systems
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		// Cycles through all the textures and attaches them to the cubemap object
		for ( int i = 0; i < 6; i++ )
		{
			TextureData texture( facesCubemap[i] , false);
			GLint internal_format = GL_RGB;
			GLenum data_format = GL_RGB;

			if ( texture.GetData() )
			{
				if ( texture.GetChannels() == 4 )
				{
					internal_format = GL_RGBA8;
					data_format = GL_RGBA;
				}
				else if ( texture.GetChannels() == 3 )
				{
					internal_format = GL_RGB8;
					data_format = GL_RGB;
				}
				else if ( texture.GetChannels() == 1 )
				{
					internal_format = GL_RGBA8;
					data_format = GL_RED;
				}
				glTexImage2D
				(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					GL_RGB,
					texture.GetWidth(),
					texture.GetHeight(),
					0,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					texture.GetData()
				);
				texture.Free();
			}
			else
			{
				std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
				texture.Free();
			}
		}
	}

	void OGLCubeMap::CubeMapEnableDepth()
	{
		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc( GL_LEQUAL );
	}

	void OGLCubeMap::CubeMapDisableDepth()
	{
		// Switch back to the normal depth function
		glDepthFunc( GL_LESS );
	}

	void OGLCubeMap::Bind()
	{
		// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		// where an object is present (a depth of 1.0f will always fail against any object's depth value)
		glBindVertexArray( m_skybox_vao );
	}

	void OGLCubeMap::Bind( uint32_t location )
	{
		assert( m_cubemap_texture != static_cast< uint32_t >( -1 ) && ( "Texture not initialized" ) );
		glActiveTexture( GL_TEXTURE0 + location );
		glBindTexture( GL_TEXTURE_CUBE_MAP, m_cubemap_texture );
	
	}

	void OGLCubeMap::Draw()
	{
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_CUBE_MAP, m_cubemap_texture );
		glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
	}

	void OGLCubeMap::UnBind()
	{
		glBindVertexArray( 0 );
	}

	unsigned int OGLCubeMap::GetTexture()
	{
		return m_cubemap_texture;
	}
}
