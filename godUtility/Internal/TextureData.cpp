#include "../pch.h"
#include "TextureData.h"

#define STB_IMAGE_IMPLEMENTATION

#pragma warning(disable: 26451)
#include <stb_image/stb_image.h>
#pragma warning(default: 26451)

namespace god
{
	TextureData::TextureData ( std::string const& texturePath , bool setFlip)
	{
		stbi_set_flip_vertically_on_load ( setFlip );
		m_data = stbi_load ( texturePath.c_str () , &m_width , &m_height , &m_channels , 0 );
	}

	/*TextureData& TextureData::operator=( TextureData const& texture )
	{
		m_data = texture.m_data;
		m_width = texture.m_width;
		m_height = texture.m_height;
	}*/

	void TextureData::Free ()
	{
		if ( m_data )
		{
			stbi_image_free ( m_data );
		}
	}

	unsigned char const* const TextureData::GetData () const
	{
		return m_data;
	}

	int TextureData::GetWidth ()
	{
		return m_width;
	}

	int TextureData::GetHeight ()
	{
		return m_height;
	}

	int TextureData::GetChannels ()
	{
		return m_channels;
	}
}