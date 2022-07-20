#pragma once

#include "godUtility.h"
#include "Internal/Model3D.h"
#include "Internal/Asset3D.h"
#include "Internal/ResourceManager.h"

#include <unordered_map>

namespace god
{
	using Asset3DManager = ResourceManager<Asset3D>;

	GODUTILITY_API Asset3D LoadAsset3D ( char const* assetPath , bool custom = false );

	struct Utility
	{

	};
}