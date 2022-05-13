#include "../pch.h"
#include "RapidJSONWrapper.h"

#include <RapidJSON/include/rapidjson/filewritestream.h>
#include <RapidJSON/include/rapidjson/filereadstream.h>
#include <RapidJSON/include/rapidjson/writer.h>

#include <cstdio>
#include <vector>
#include <iostream>

namespace god
{
	namespace RapidJSON
	{
		void Read ( rapidjson::Document& document , char const* json )
		{
			using namespace rapidjson;

			FILE* fp;
			fopen_s ( &fp , json , "rb" );
			if ( fp )
			{
				std::vector<char> read_buffer ( READ_BUFFER_SIZE );
				FileReadStream is ( fp , read_buffer.data () , read_buffer.size () );

				document.ParseStream ( is );

				fclose ( fp );
			}
			else
			{
				std::cerr << "god::RapidJSON::Read - Could not open json file." << std::endl;
			}
		}

		void Write ( rapidjson::Document const& document , char const* json )
		{
			using namespace rapidjson;

			FILE* fp;
			fopen_s ( &fp , json , "wb" );

			if ( fp )
			{
				std::vector<char> write_buffer ( READ_BUFFER_SIZE );
				FileWriteStream os ( fp , write_buffer.data () , write_buffer.size () );

				Writer<FileWriteStream> writer ( os );
				document.Accept ( writer );

				fclose ( fp );
			}
			else
			{
				std::cerr << "god::RapidJSON::Write - Could not open json file." << std::endl;
			}
		}
	}
}