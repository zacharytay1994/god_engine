#pragma once

#include <string>

namespace god
{
	namespace AssetPath
	{
		const std::string Folder_3DAssets { "Assets/GameAssets/3DAssets/" };
		const std::string Folder_2DAssets { "Assets/GameAssets/2DAssets/" };
		const std::string Folder_Assets { "Assets/GameAssets/" };
		const std::string Folder_Raw { "Raw/" };
		const std::string Folder_Build { "Build/" };
		const std::string Folder_Models { "Models/" };
		const std::string Folder_Textures { "Textures/" };
		const std::string Folder_Sounds { "Sounds/" };

		const std::string Folder_RawModels { Folder_3DAssets + Folder_Raw + Folder_Models };
		const std::string Folder_BuildModels { Folder_3DAssets + Folder_Build + Folder_Models };

		const std::string Folder_RawTextures { Folder_2DAssets + Folder_Raw + Folder_Textures };
		const std::string Folder_BuildTextures { Folder_2DAssets + Folder_Build + Folder_Textures };

		const std::string Folder_BuildSounds{ Folder_Assets + Folder_Sounds };

		const std::string File_ModelsConfig { Folder_3DAssets + "ModelsConfig.json" };
		const std::string File_TexturesConfig { Folder_2DAssets + "TexturesConfig.json" };
		const std::string File_SoundsConfig { Folder_BuildSounds + "SoundsConfig.json" };
	}
}