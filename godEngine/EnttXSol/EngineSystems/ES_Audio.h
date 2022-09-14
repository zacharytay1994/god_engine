#pragma once

#include "../EngineComponents/EC_All.h"
#include "../../Audio/AudioAPI.h"

bool played = false;

FMOD::Sound* sound;

namespace god
{
	void AudioSystem(EnttXSol& entt, std::tuple<EntityData&, AudioSource&> components)
	{
		auto& entity_data = std::get<0>(components);
		auto& audio = std::get<1>(components);

		if (!played)
		{
			AudioAPI::LoadSound("C:\\Users\\Tingl\\Desktop\\CSD3400\\god_engine\\1_Game\\Assets\\GameAssets\\Sounds\\649088__davejf__melody-loop-105-bpm.mp3", &sound);

			AudioAPI::Play(sound);
			played = true;
		}
	}
}