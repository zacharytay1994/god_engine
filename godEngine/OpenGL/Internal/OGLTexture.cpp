#include "../../pch.h"
#include "OGLTexture.h"
#include "gli.hpp"

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#pragma comment (lib, "opengl32.lib")

#include <godUtility/Internal/TextureData.h>
#include <godUtility/FileIO.h>
#include <godUtility/Internal/RapidJSONWrapper.h>

#include "OGLDebug.h"

#include <filesystem>
// to bypass windows.h GetObject() macro definition
#ifdef _MSC_VER
#undef GetObject
#endif


namespace god
{
	OGLTexture::OGLTexture (std::string const& texturePath, bool DDS)
	{
		DDS = (std::filesystem::path(texturePath).extension().string() == ".dds");
		isDDs = DDS;
		if (isDDs)
		{
			char const* dds_path = texturePath.c_str();

			gli::texture DDS_Texture = gli::load(dds_path); // Can load DDS, KTX or KMG

			if (DDS_Texture.empty())
				std::cerr << " Failed to load DDS texture!" << dds_path << "\n";


			gli::gl GL(gli::gl::PROFILE_GL33);
			gli::gl::format const Format = GL.translate(DDS_Texture.format(), DDS_Texture.swizzles());
			GLenum Target = GL.translate(DDS_Texture.target());
			assert(gli::is_compressed(DDS_Texture.format()) && Target == gli::gl::target::TARGET_2D);

			GLuint TextureName = 0;
			
			glGenTextures(1, &TextureName);
			glBindTexture(Target, TextureName);
			
			glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(DDS_Texture.levels() - 1));
			glTexParameteriv(Target, GL_TEXTURE_SWIZZLE_RGBA, &Format.Swizzles[0]);
			
			for (std::size_t Level = 0; Level < DDS_Texture.levels(); ++Level)
			{
				glm::tvec3<GLsizei> Extent(DDS_Texture.extent(Level));

				glTexStorage2D(Target, static_cast<GLint>(DDS_Texture.levels()), Format.Internal, Extent.x, Extent.y);
				
				glCompressedTexSubImage2D(
					Target, static_cast<GLint>(Level), 0, 0, Extent.x, Extent.y,
					Format.Internal, static_cast<GLsizei>(DDS_Texture.size(Level)), DDS_Texture.data(0, 0, Level));

				
			}
			m_texture_id = TextureName;

			
		}
		else
		{
			TextureData texture(texturePath);

			glCreateTextures(GL_TEXTURE_2D, 1, &m_texture_id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			GLint internal_format = GL_RGB;
			GLenum data_format = GL_RGB;

			if (texture.GetData())
			{
				if (texture.GetChannels() == 4)
				{
					internal_format = GL_SRGB8_ALPHA8;
					data_format = GL_RGBA;
				}
				else if (texture.GetChannels() == 3)
				{
					internal_format = GL_SRGB8;
					data_format = GL_RGB;
				}
				else if (texture.GetChannels() == 1)
				{
					internal_format = GL_RGBA8;
					data_format = GL_RED;
				}
				glTextureStorage2D(m_texture_id, 1, internal_format, texture.GetWidth(), texture.GetHeight());
				glTextureSubImage2D(m_texture_id, 0, 0, 0, texture.GetWidth(), texture.GetHeight(), data_format, GL_UNSIGNED_BYTE, texture.GetData());
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cerr << "OGLTexture:: - Failed to load texture " << texturePath << std::endl;
			}

			texture.Free();
		}
		

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
		manager.Insert("myddstest", { 1, OGLTexture("Assets/EngineAssets/Textures/test.dds" , true) });

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
						manager.Insert ( model.name.GetString () , { 0, OGLTexture ( assetFolderPath + model.value.GetArray ()[ 0 ][ "Raw" ].GetString ()) } );
						
					}
				}
			}
		}
	}
}