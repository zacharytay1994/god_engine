#pragma once
#include "../godUtility.h"

#include <string>
#include <vector>

namespace god
{
	namespace FolderHelper
	{
		static std::string const FOLDERNAME_RESOURCE { "crypetals_res" };
		static std::string const FOLDERNAME_ASSETS { FOLDERNAME_RESOURCE + "/assets" };
		static std::string const FOLDERNAME_CONFIG { FOLDERNAME_RESOURCE + "/config" };
		static std::string const FILENAME_SETTINGS { FOLDERNAME_CONFIG + "/settings.txt"};

		bool FileExists ( const char* filename );

		void CopyFileToAssets ( const wchar_t* filepath , const char* dirpath );

		void GODUTILITY_API CopyFileToFolder ( std::string const& filePath , std::string const& folderPath );

		std::vector<std::string> GetDirectories ( const std::string& dir );

		std::vector<std::string> GODUTILITY_API GetFiles ( const std::string& dir );
	}
}