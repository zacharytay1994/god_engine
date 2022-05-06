#pragma once

#include "godUtility.h"

#include <string>
#include <vector>

namespace god
{
	std::string GODUTILITY_API ReadFileToString ( char const* file );

	std::vector<char> GODUTILITY_API ReadSPVToBuffer ( char const* spv );
}