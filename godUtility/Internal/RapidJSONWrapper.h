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

		void Read ( rapidjson::Document& document , std::string const& json );

		void Write ( rapidjson::Document const& document , std::string const& json );

		template <typename VALUE>
		void JSONify ( rapidjson::Document& document , std::string const& key , VALUE& value )
		{
			rapidjson::Value rj_value ( value );
			rapidjson::Value rj_key ( key.c_str () , document.GetAllocator () );
			document.AddMember ( rj_key , rj_value , document.GetAllocator () );
		}
	
		template <>
		inline void JSONify<rapidjson::Value> ( rapidjson::Document& document , std::string const& key , rapidjson::Value& value )
		{
			rapidjson::Value rj_key ( key.c_str () , document.GetAllocator () );
			document.AddMember ( rj_key , value , document.GetAllocator () );
		}

		template <typename...VALUES>
		void JSONifyValues ( rapidjson::Document& document , std::string const& key , VALUES&& ... values )
		{
			rapidjson::Value rj_value ( rapidjson::kArrayType );
			rapidjson::Value rj_key ( key.c_str () , document.GetAllocator () );
			( ( rj_value.PushBack ( values , document.GetAllocator () ) ) , ... );
			document.AddMember ( rj_key , rj_value , document.GetAllocator () );
		}

		template <typename VALUE>
		void JSONifyToValue ( rapidjson::Value& rjValue , rapidjson::Document& document , std::string const& key , VALUE const& value )
		{
			rapidjson::Value rj_value ( value );
			rapidjson::Value rj_key ( key.c_str () , document.GetAllocator () );
			rjValue.AddMember ( rj_key , rj_value , document.GetAllocator () );
		}

		template <typename...VALUES>
		void JSONifyValues ( rapidjson::Value& rjValue , rapidjson::Document& document , std::string const& key , VALUES&& ... values )
		{
			rapidjson::Value rj_value ( rapidjson::kArrayType );
			rapidjson::Value rj_key ( key.c_str () , document.GetAllocator () );
			( ( rj_value.PushBack ( values , document.GetAllocator () ) ) , ... );
			rjValue.AddMember ( rj_key , rj_value , document.GetAllocator () );
		}
	}
}