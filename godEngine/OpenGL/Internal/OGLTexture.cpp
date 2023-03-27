#include "../../pch.h"
#include "OGLTexture.h"
#include "gli.hpp"
#include "texture.hpp"
#include "load.hpp"
#include "save.hpp"


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

#pragma warning(disable : 4996)
namespace god
{
	GLuint ddsLoader(const char* Filename)
	{
		gli::texture Texture = gli::load(Filename);

		if (Texture.empty())
			return 0;

		if(Texture.format())
			Texture = gli::flip(Texture);

		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
		GLenum Target = GL.translate(Texture.target());
		GLuint TextureName = 0;
		glGenTextures(1, &TextureName);
		glBindTexture(Target, TextureName);
		glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
		glm::tvec3<GLsizei> const Dimensions(Texture.extent());
		GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());
		switch (Texture.target())
		{
		case gli::TARGET_1D:
			glTexStorage1D(Target, static_cast<GLint>(Texture.levels()), Format.Internal, Dimensions.x);
			break;
		case gli::TARGET_1D_ARRAY:
		case gli::TARGET_2D:
		case gli::TARGET_CUBE:
			glTexStorage2D(
				Target, static_cast<GLint>(Texture.levels()), Format.Internal,
				Dimensions.x, Texture.target() == gli::TARGET_2D ? Dimensions.y : FaceTotal);
			break;
		case gli::TARGET_2D_ARRAY:
		case gli::TARGET_3D:
		case gli::TARGET_CUBE_ARRAY:
			glTexStorage3D(
				Target, static_cast<GLint>(Texture.levels()), Format.Internal,
				Dimensions.x, Dimensions.y, Texture.target() == gli::TARGET_3D ? Dimensions.z : FaceTotal);
			break;
		default: assert(0); break;
		}
		for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
			for (std::size_t Face = 0; Face < Texture.faces(); ++Face)
				for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
				{
					GLsizei const LayerGL = static_cast<GLsizei>(Layer);
					glm::tvec3<GLsizei> Dimensions_(Texture.extent(Level));
					if (gli::is_target_cube(Texture.target()))
						Target = static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face);
					switch (Texture.target())
					{
					case gli::TARGET_1D:
						if (gli::is_compressed(Texture.format()))
							glCompressedTexSubImage1D(
								Target, static_cast<GLint>(Level), 0, Dimensions_.x,
								Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
								Texture.data(Layer, Face, Level));
						else
							glTexSubImage1D(
								Target, static_cast<GLint>(Level), 0, Dimensions_.x, Format.External, Format.Type,
								Texture.data(Layer, Face, Level));
						break;
					case gli::TARGET_1D_ARRAY:
					case gli::TARGET_2D:
					case gli::TARGET_CUBE:
						if (gli::is_compressed(Texture.format()))
							glCompressedTexSubImage2D(
								Target, static_cast<GLint>(Level), 0, 0,
								Dimensions_.x, Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Dimensions_.y,
								Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
								Texture.data(Layer, Face, Level));
						else
							glTexSubImage2D(
								Target, static_cast<GLint>(Level), 0, 0,
								Dimensions_.x, Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Dimensions_.y,
								Format.External, Format.Type, Texture.data(Layer, Face, Level));
						break;
					case gli::TARGET_2D_ARRAY:
					case gli::TARGET_3D:
					case gli::TARGET_CUBE_ARRAY:
						if (gli::is_compressed(Texture.format()))
							glCompressedTexSubImage3D(
								Target, static_cast<GLint>(Level), 0, 0, 0,
								Dimensions_.x, Dimensions_.y, Texture.target() == gli::TARGET_3D ? Dimensions_.z : LayerGL,
								Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
								Texture.data(Layer, Face, Level));
						else
							glTexSubImage3D(
								Target, static_cast<GLint>(Level), 0, 0, 0,
								Dimensions_.x, Dimensions_.y, Texture.target() == gli::TARGET_3D ? Dimensions_.z : LayerGL,
								Format.External, Format.Type, Texture.data(Layer, Face, Level));
						break;
					default: assert(0); break;
					}
				}
		return TextureName;
	
	}

	OGLTexture::OGLTexture (std::string const& texturePath, bool DDS)
	{
		DDS = (std::filesystem::path(texturePath).extension().string() == ".dds");
		isDDs = DDS;
		if (isDDs)
		{
			char const* dds_path = texturePath.c_str();
			GLuint id = ddsLoader(dds_path);
			if (!id)
			{
				std::cout << "Dds fail!\n";
			}

			m_texture_id = id;
			//char const* dds_path = texturePath.c_str();

			//gli::texture DDS_Texture = gli::load(dds_path); // Can load DDS, KTX or KMG

			//if (DDS_Texture.empty())
			//	std::cerr << " Failed to load DDS texture!" << dds_path << "\n";


			//gli::gl GL(gli::gl::PROFILE_GL33);
			//gli::gl::format const Format = GL.translate(DDS_Texture.format(), DDS_Texture.swizzles());
			//GLenum Target = GL.translate(DDS_Texture.target());
			//assert(gli::is_compressed(DDS_Texture.format()) && Target == gli::gl::target::TARGET_2D);

			//GLuint TextureName = 0;
			//
			//glGenTextures(1, &TextureName);
			//glBindTexture(Target, TextureName);
			//
			//glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
			//glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(DDS_Texture.levels() - 1));
			//glTexParameteriv(Target, GL_TEXTURE_SWIZZLE_RGBA, &Format.Swizzles[0]);
			//
			//for (std::size_t Level = 0; Level < DDS_Texture.levels(); ++Level)
			//{
			//	glm::tvec3<GLsizei> Extent(DDS_Texture.extent(Level));

			//	glTexStorage2D(Target, static_cast<GLint>(DDS_Texture.levels()), Format.Internal, Extent.x, Extent.y);
			//	
			//	glCompressedTexSubImage2D(
			//		Target, static_cast<GLint>(Level), 0, 0, Extent.x, Extent.y,
			//		Format.Internal, static_cast<GLsizei>(DDS_Texture.size(Level)), DDS_Texture.data(0, 0, Level));

			//	
			//}
			//m_texture_id = TextureName;

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