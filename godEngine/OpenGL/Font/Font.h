#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

#include <freetype/freetype/freetype.h>

namespace god
{
	struct Character
	{
		uint32_t	m_texture_ID;	// ID handle of the glyph texture
		glm::ivec2  m_size;			// Size of glyph
		glm::ivec2  m_bearing;		// Offset from baseline to left/top of glyph
		uint32_t	m_advance;		// Offset to advance to next glyph, in pixel >> 6
	};

	struct Font
	{
		Font ( FT_Library library , std::string const& path , uint32_t defaultSize );

		void Free ();
		std::vector<Character> const& GetCharacters ( uint32_t size ) const;
		void InitializeSizedCharacters ( uint32_t height );

	private:
		FT_Face m_face { nullptr };
		std::unordered_map<uint32_t , std::vector<Character>> m_characters;
	};

	struct Fonts
	{
		Fonts ();
		~Fonts ();

		void InsertNewFont ( std::string const& name , std::string const& path );
		Font const& GetFont ( std::string const& name ) const;

		static uint32_t DEFAULT_FONT_SIZE;
	private:
		FT_Library m_library { nullptr };
		std::unordered_map<std::string , Font> m_fonts;
	};
}