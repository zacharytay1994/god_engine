#pragma once

#include "../EngineComponents/EC_All.h"

namespace god
{
	void AudioSystem(EnttXSol& entt, EngineResources& engineResources, std::tuple<EntityData&, AudioSource&> components)
	{
		(entt); (engineResources);

		auto& entity_data = std::get<0>(components);
		auto& audio = std::get<1>(components);

		if (!audio.m_sound.m_sound_sample)
		{
			if (entity_data.m_id == 0)
				AudioAPI::LoadSound("C:\\Users\\Tingl\\Desktop\\CSD3400\\god_engine\\1_Game\\Assets\\GameAssets\\Sounds\\649088__davejf__melody-loop-105-bpm.mp3", audio.m_sound);
			if (entity_data.m_id == 1)
				AudioAPI::LoadSound("C:\\Users\\Tingl\\Desktop\\CSD3400\\god_engine\\1_Game\\Assets\\GameAssets\\Sounds\\81239__jesterdyne__crash1-hihi.wav", audio.m_sound);
		}

		AudioAPI::SetLoop(audio.m_sound, audio.m_loop);
		AudioAPI::SetMute(audio.m_sound, audio.m_mute);
		AudioAPI::SetVolume(audio.m_sound, audio.m_volume);
		AudioAPI::SetPitch(audio.m_sound, audio.m_pitch);

		if (audio.m_play_on_awake && !audio.m_sound.m_played)
		{
			AudioAPI::Play(audio.m_sound);
		}
	}
}