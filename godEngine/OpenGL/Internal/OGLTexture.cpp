#include "../../pch.h"
#include "OGLTexture.h"

#include <glad/glad.h>
#pragma comment (lib, "opengl32.lib")

#include <godUtility/Internal/TextureData.h>
#include <godUtility/FileIO.h>
#include <godUtility/Internal/RapidJSONWrapper.h>

// to bypass windows.h GetObject() macro definition
#ifdef _MSC_VER
#undef GetObject
#endif

namespace god
{
	OGLTexture::OGLTexture ( std::string const& texturePath )
	{
		TextureData texture ( texturePath );

		glCreateTextures ( GL_TEXTURE_2D , 1 , &m_texture_id );

		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT );
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT );
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );

		GLint internal_format = GL_RGB;
		GLenum data_format = GL_RGB;

		if ( texture.GetData () )
		{
			if ( texture.GetChannels () == 4 )
			{
				//internal_format = GL_RGBA8;
				internal_format = GL_SRGB_ALPHA; //--
				data_format = GL_RGBA;
			}
			else if ( texture.GetChannels () == 3 )
			{
				//internal_format = GL_RGB8;
				internal_format = GL_SRGB;
				data_format = GL_RGB;
			}
			else if ( texture.GetChannels () == 1 )
			{
				internal_format = GL_RGBA8;
				data_format = GL_RED;
			}
			glTextureStorage2D ( m_texture_id , 1 , internal_format , texture.GetWidth () , texture.GetHeight () );
			glTextureSubImage2D ( m_texture_id , 0 , 0 , 0 , texture.GetWidth () , texture.GetHeight () , data_format , GL_UNSIGNED_BYTE , texture.GetData () );
			glGenerateMipmap ( GL_TEXTURE_2D );
		}
		else
		{
			std::cerr << "OGLTexture:: - Failed to load texture " << texturePath << std::endl;
		}

		texture.Free ();
	}

	void OGLTexture::Free ()
	{
		glDeleteTextures ( 1 , &m_texture_id );
	}

	void OGLTexture::Bind ( uint32_t location ) const
	{
		assert ( m_texture_id != static_cast< uint32_t >( -1 ) && ( "Texture not initialized" ) );
		glActiveTexture ( GL_TEXTURE0 + location );
		glBindTexture ( GL_TEXTURE_2D , m_texture_id );
	}

	void OGLTexture::UnBind ( uint32_t location ) const
	{
		glActiveTexture ( GL_TEXTURE0 + location );
		glBindTexture ( GL_TEXTURE_2D , 0 );
	}

	void InsertEngineOGLTextures ( OGLTextureManager& manager )
	{
		// insert "none" image
		manager.Insert ( "None" , { 0, OGLTexture ( "Assets/EngineAssets/Textures/BlackTexture.png" ) } );
	}

	void InsertAllOGLTexturesFromConfig ( std::string const& configPath , std::string const& assetFolderPath , OGLTextureManager& manager )
	{
		rapidjson::Document m_document_models;
		m_document_models.SetObject ();
		god::ReadJSON ( m_document_models , configPath );
		if ( m_document_models.IsObject () )
		{
			for ( auto& model : m_document_models.GetObject () )
			{
				if ( !manager.Has ( model.name.GetString () ) )
				{
					if ( model.value[ 0 ].HasMember ( "UID" ) )
					{
						manager.Insert ( model.name.GetString () , { model.value[ 0 ][ "UID" ].GetUint (), OGLTexture ( assetFolderPath + model.value.GetArray ()[ 0 ][ "Raw" ].GetString () ) } );
					}
					else
					{
						manager.Insert ( model.name.GetString () , { 0, OGLTexture ( assetFolderPath + model.value.GetArray ()[ 0 ][ "Raw" ].GetString () ) } );
					}
				}
			}
		}
	}
}