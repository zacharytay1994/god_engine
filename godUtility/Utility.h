#pragma once

#include "godUtility.h"
#include "Internal/AssimpWrapper.h"
#include "Internal/ResourceManager.h"

#include <unordered_map>

namespace god
{
	using AssimpModelManager = ResourceManager<AssimpModel>;

	GODUTILITY_API AssimpModel LoadModel ( char const* modelPath );

	struct Utility
	{
		
	};
}