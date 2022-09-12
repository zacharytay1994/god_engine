#pragma once

#include "../godUtility.h"

#include <string>

namespace god
{
	struct TextureData
	{
		GODUTILITY_API TextureData ( std::string const& texturePath, bool setFlip = true );
		//GODUTILITY_API TextureData& operator=( TextureData const& texture );
		GODUTILITY_API void Free ();

		GODUTILITY_API unsigned char const* const GetData () const;
		GODUTILITY_API int GetWidth ();
		GODUTILITY_API int GetHeight ();
		GODUTILITY_API int GetChannels ();

	private:
		unsigned char* m_data { nullptr };
		int m_width { 0 } , m_height { 0 } , m_channels { 0 };
	};
}