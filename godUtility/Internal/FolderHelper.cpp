#include "../pch.h"
#include "FolderHelper.h"

namespace god
{
	namespace FolderHelper
	{
		bool FileExists ( const char* filename )
		{
			struct stat buf;
			if ( stat ( filename , &buf ) != -1 )
			{
				return true;
			}
			return false;
		}

		void CopyFileToAssets ( const wchar_t* filepath , const char* foldername )
		{
			// create assets directory if doesnt exist
			if ( !std::filesystem::is_directory ( FOLDERNAME_ASSETS ) )
			{
				std::filesystem::create_directory ( FOLDERNAME_ASSETS );
			}
			std::stringstream dirpath;
			dirpath << FOLDERNAME_ASSETS << "/" << foldername;
			if ( !std::filesystem::is_directory ( dirpath.str () ) )
			{
				std::filesystem::create_directory ( dirpath.str () );
			}

			// convert to string and find the name
			std::wstring file { filepath };
			std::string sfile ( file.begin () , file.end () );
			sfile = sfile.substr ( sfile.find_last_of ( '\\' ) + 1 , file.size () );
			std::stringstream ss;
			ss << dirpath.str () << "/" << sfile;
			std::cout << ss.str () << std::endl;

			std::ifstream src ( filepath , std::ios::binary );
			std::ofstream dest ( ss.str ().c_str () , std::ios::binary );
			dest << src.rdbuf ();
		}

		std::vector<std::string> GetDirectories ( const std::string& s )
		{
			std::vector<std::string> r;
			for ( auto& p : std::filesystem::recursive_directory_iterator ( s ) )
				if ( p.is_directory () )
					r.push_back ( p.path ().string () );
			return r;
		}

		std::vector<std::string> GetFiles ( const std::string& dir )
		{
			std::vector<std::string> files;
			for ( const auto& entry : std::filesystem::directory_iterator ( dir ) )
				files.push_back ( entry.path ().string () );
			return files;
		}
	}
}