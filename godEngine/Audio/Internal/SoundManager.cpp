#include "../../pch.h"
#include "SoundManager.h"

#include <godUtility/FileIO.h>
#include <godUtility/Internal/RapidJSONWrapper.h>

// to bypass windows.h GetObject() macro definition
#ifdef _MSC_VER
#undef GetObject
#endif

namespace god
{
	void InsertAllSoundsFromConfig(std::string const& configPath, std::string const& assetFolderPath, SoundManager& manager)
	{
		rapidjson::Document m_document_sounds;
		m_document_sounds.SetObject();
		god::ReadJSON(m_document_sounds, configPath);
		if (m_document_sounds.IsObject())
		{
			for (auto& sound : m_document_sounds.GetObject())
			{
				if (!manager.Has(sound.name.GetString()))
				{
					if (sound.value[0].HasMember("UID"))
					{
						manager.Insert(sound.name.GetString(), { sound.value[0]["UID"].GetUint(), Sound(assetFolderPath + sound.value.GetArray()[0]["Raw"].GetString()) });
					}
					else
					{
						manager.Insert(sound.name.GetString(), { 0, Sound(assetFolderPath + sound.value.GetArray()[0]["Raw"].GetString()) });
					}
				}
			}
		}
	}
}