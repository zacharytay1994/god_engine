#pragma once

#include <godUtility/Internal/ResourceManager.h>
#include "../AudioAPI.h"

namespace god
{
	using SoundManager = ResourceManager<std::tuple<uint32_t, Sound>>;

	void InsertAllSoundsFromConfig(std::string const& configPath, std::string const& assetFolderPath, SoundManager& manager);
}