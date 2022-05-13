#pragma once

#include <RapidJSON/include/rapidjson/document.h>

#include <string>
#include <utility>
#include <iostream>

namespace god
{
	namespace RapidJSON
	{
		static constexpr uint32_t READ_BUFFER_SIZE = 65536;

		void Read ( rapidjson::Document& document , char const* json );

		void Write ( rapidjson::Document const& document , char const* json );

		template <typename OBJECT , typename VALUE>
		void JSONify ( OBJECT& object , std::string const& key , VALUE const& value )
		{
			rapidjson::Value rj_value ( value );
			rapidjson::Value rj_key ( key.c_str () , object.GetAllocator () );
			object.AddMember ( rj_key , rj_value , object.GetAllocator () );
		}

		template <typename OBJECT , typename...VALUES>
		void JSONifyValues ( OBJECT& object , std::string const& key , VALUES&& ... values )
		{
			rapidjson::Value rj_value ( rapidjson::kArrayType );
			rapidjson::Value rj_key ( key.c_str () , object.GetAllocator () );
			( ( rj_value.PushBack ( values , object.GetAllocator () ) ) , ... );
			object.AddMember ( rj_key , rj_value , object.GetAllocator () );
		}
	}
}