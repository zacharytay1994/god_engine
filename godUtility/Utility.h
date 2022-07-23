#pragma once

#include "godUtility.h"
#include "Internal/Model3D.h"
#include "Internal/Asset3D.h"
#include "Internal/ResourceManager.h"

#include <unordered_map>
#include <string>

namespace god
{
	using Asset3DManager = ResourceManager<Asset3D>;

	GODUTILITY_API Asset3D LoadAsset3D ( std::string const& assetPath , bool custom = false );

	GODUTILITY_API void InsertAsset3DFromPath ( std::string const& assetName , std::string const& assetFolderPath , Asset3DManager& manager );

	GODUTILITY_API void InsertAllAsset3DsFromConfig ( std::string const& configPath , std::string const& assetFolderPath , Asset3DManager& manager );

	struct Utility
	{

	};
}