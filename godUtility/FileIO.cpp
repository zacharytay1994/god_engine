#include "pch.h"
#include "FileIO.h"

#include "Internal/RapidJSONWrapper.h"

namespace god
{
	std::string ReadFileToString ( char const* file )
	{
		std::ifstream input_file ( file );
		assert ( input_file && "god::ReadFileToString - Could not read input file." );
		std::stringstream buffer;
		buffer << input_file.rdbuf ();
		input_file.close ();
		return buffer.str ();
	}

	std::vector<char> ReadSPVToBuffer ( char const* spv )
	{
		std::ifstream file ( spv , std::ios::ate | std::ios::binary );

		assert ( file && "god::ReadSPVToBuffer - Could not read spv file." );

		size_t fileSize = static_cast< size_t >( file.tellg () );
		std::vector<char> buffer ( fileSize );

		file.seekg ( 0 );
		file.read ( buffer.data () , fileSize );
		file.close ();

		return buffer;
	}

	void ReadJSON ( rapidjson::Document& document , std::string const& json )
	{
		RapidJSON::Read ( document , json );
	}

	void WriteJSON ( rapidjson::Document const& document , std::string const& json )
	{
		RapidJSON::Write ( document , json );
	}

	std::string OpenWindowDialog ( wchar_t const* fileType , wchar_t const* fileExt ,
		WindowsDialog::MODE mode , const char* dir )
	{
		return DialogBoxToDirectory ( fileType , fileExt , mode , dir );
	}

	std::vector<std::string> OpenWindowDialogMulti ( wchar_t const* fileType , wchar_t const* fileExt ,
		WindowsDialog::MODE mode , const char* dir )
	{
		return DialogBoxToDirectoryMulti ( fileType , fileExt , mode , dir );
	}
}