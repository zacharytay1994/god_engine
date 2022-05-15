#pragma once

#include <string>

namespace god
{
	namespace WindowsDialog
	{
		enum class MODE
		{
			GETPATH ,
			ASSETS
		};

		std::string DialogBoxToDirectory ( wchar_t const* fileType = L"All Files" , wchar_t const* fileExt = L"*.*" ,
			MODE mode = MODE::GETPATH , const char* dir = 0 );
	}
}