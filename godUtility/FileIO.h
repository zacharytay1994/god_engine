#pragma once

#include "godUtility.h"

#include <RapidJSON/include/rapidjson/document.h>

#include <string>
#include <vector>

namespace god
{
	std::string GODUTILITY_API ReadFileToString ( char const* file );

	std::vector<char> GODUTILITY_API ReadSPVToBuffer ( char const* spv );

	void GODUTILITY_API ReadJSON ( rapidjson::Document& document , std::string const& json );

	void GODUTILITY_API WriteJSON ( rapidjson::Document const& document , std::string const& json );
}