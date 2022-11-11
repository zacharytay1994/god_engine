#include "../../pch.h"
#include "Font.h"

#include <glad/glad.h>
#pragma comment(lib, "opengl32.lib")

#ifdef _DEBUG
#pragma comment (lib, "freetype.lib")
#else
#pragma comment (lib, "freetype.lib")
#endif

namespace god
{
	Font::Font ( FT_Library library , std::string const& path , uint32_t defaultSize )
	{
		if ( FT_New_Face ( library , path.c_str () , 0 , &m_face ) )
		{
			std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
		}

		// load characters of default size
		InitializeSizedCharacters ( defaultSize );
	}

	void Font::Free ()
	{
		FT_Done_Face ( m_face );
	}

	std::vector<Character> const& Font::GetCharacters ( uint32_t size ) const
	{
		assert ( m_characters.find ( size ) != m_characters.end () );
		return m_characters.at ( size );
	}

	void Font::InitializeSizedCharacters ( uint32_t height )
	{
		FT_Set_Pixel_Sizes ( m_face , 0 , height );
		m_characters.insert ( { height,{} } );
		auto& characters = m_characters[ height ];
		characters.resize ( 128 );

		glPixelStorei ( GL_UNPACK_ALIGNMENT , 1 ); // disable byte-alignment restriction

		for ( unsigned char c = 0; c < 128; c++ )
		{
			// load character glyph 
			if ( FT_Load_Char ( m_face , c , FT_LOAD_RENDER ) )
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures ( 1 , &texture );
			glBindTexture ( GL_TEXTURE_2D , texture );
			glTexImage2D (
				GL_TEXTURE_2D ,
				0 ,
				GL_RED ,
				m_face->glyph->bitmap.width ,
				m_face->glyph->bitmap.rows ,
				0 ,
				GL_RED ,
				GL_UNSIGNED_BYTE ,
				m_face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE );
			glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE );
			glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );
			glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2 ( m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows ),
				glm::ivec2 ( m_face->glyph->bitmap_left, m_face->glyph->bitmap_top ),
				( m_face->glyph->advance.x >> 6 )
			};
			characters[ static_cast< uint32_t >( c ) ] = character;
		}
	}

	uint32_t Fonts::DEFAULT_FONT_SIZE { 100 };

	Fonts::Fonts ()
	{
		if ( FT_Init_FreeType ( &m_library ) )
		{
			std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		}

		std::cout << "Fonts - Initialized" << std::endl;
	}

	Fonts::~Fonts ()
	{
		for ( auto& font : m_fonts )
		{
			font.second.Free ();
		}
		FT_Done_FreeType ( m_library );
	}

	void Fonts::InsertNewFont ( std::string const& name , std::string const& path )
	{
		m_fonts.insert ( { name, { m_library , path , DEFAULT_FONT_SIZE } } );
		std::cout << "Font - " << name << " initialized." << std::endl;
	}

	Font const& Fonts::GetFont ( std::string const& name ) const
	{
		assert ( m_fonts.find ( name ) != m_fonts.end () );
		return m_fonts.at ( name );
	}
}