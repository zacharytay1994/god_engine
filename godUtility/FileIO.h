#pragma once

#include "godUtility.h"

#include "Internal/RapidJSONWrapper.h"

#include <string>
#include <vector>

namespace god
{
	std::string GODUTILITY_API ReadFileToString ( char const* file );

	std::vector<char> GODUTILITY_API ReadSPVToBuffer ( char const* spv );

	void GODUTILITY_API ReadJSON ( rapidjson::Document& document , char const* json );

	void GODUTILITY_API WriteJSON ( rapidjson::Document const& document , char const* json );
}