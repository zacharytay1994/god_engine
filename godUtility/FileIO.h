#pragma once

#include "godUtility.h"
#include "Internal/WindowsDialog.h"
#include "Internal/FolderHelper.h"

#include <RapidJSON/include/rapidjson/document.h>

#include <string>
#include <vector>

namespace god
{
	std::string GODUTILITY_API ReadFileToString ( char const* file );

	void GODUTILITY_API WriteStringToFile ( std::string const& file , std::string const& content );

	std::vector<char> GODUTILITY_API ReadSPVToBuffer ( char const* spv );

	void GODUTILITY_API ReadJSON ( rapidjson::Document& document , std::string const& json );

	void GODUTILITY_API WriteJSON ( rapidjson::Document const& document , std::string const& json );

	std::string GODUTILITY_API OpenWindowDialog ( wchar_t const* fileType = L"All Files" , wchar_t const* fileExt = L"*.*" ,
		WindowsDialog::MODE mode = WindowsDialog::MODE::GETPATH , const char* dir = 0 );

	std::vector<std::string> GODUTILITY_API OpenWindowDialogMulti ( wchar_t const* fileType = L"All Files" , wchar_t const* fileExt = L"*.*" ,
		WindowsDialog::MODE mode = WindowsDialog::MODE::GETPATH , const char* dir = 0 );

	void GODUTILITY_API DeleteFileAtPath ( std::string const& filePath );

	bool GODUTILITY_API FileExists ( std::string const& folder , std::string const& file );
}