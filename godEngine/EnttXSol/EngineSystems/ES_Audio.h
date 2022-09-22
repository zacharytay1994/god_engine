#pragma once

#include "../EngineComponents/EC_All.h"
#include "../../Audio/AudioAPI.h"

bool playing = false;

namespace god
{
	//void AudioSystem(EnttXSol& entt, EngineResources& engineResources, std::tuple<EntityData&, AudioSource&> components)
	//{
	//	(entt);

	//	// engine resources access
	//	SoundManager& sound_manager = engineResources.Get<SoundManager>().get();

	//	auto& entity_data = std::get<0>(components);
	//	auto& audio = std::get<1>(components);

	//	if (audio.m_sound_id != -1)
	//	{
	//		auto& resource = sound_manager.Get(audio.m_sound_id);
	//		Sound& sound = std::get<1>(resource);

	//		//if (!audio.m_sound.m_sound_sample)
	//		//{
	//		//	if (entity_data.m_id == 0)
	//		//		AudioAPI::LoadSound("C:\\Users\\Tingl\\Desktop\\CSD3400\\god_engine\\1_Game\\Assets\\GameAssets\\Sounds\\649088__davejf__melody-loop-105-bpm.mp3", audio.m_sound);
	//		//	if (entity_data.m_id == 1)
	//		//		AudioAPI::LoadSound("C:\\Users\\Tingl\\Desktop\\CSD3400\\god_engine\\1_Game\\Assets\\GameAssets\\Sounds\\81239__jesterdyne__crash1-hihi.wav", audio.m_sound);
	//		//}

	//		//AudioAPI::SetLoop(audio.m_sound, audio.m_loop);
	//		//AudioAPI::SetMute(audio.m_sound, audio.m_mute);
	//		//AudioAPI::SetVolume(audio.m_sound, audio.m_volume);
	//		//AudioAPI::SetPitch(audio.m_sound, audio.m_pitch);

	//		//if (audio.m_play_on_awake && !audio.m_sound.m_played)
	//		//{
	//		//	AudioAPI::Play(audio.m_sound);
	//		//}

	//		AudioAPI::SetLoop(sound, audio.m_loop);

	//		if (audio.m_play_on_awake && !sound.m_played)
	//		{
	//			AudioAPI::Play(sound);
	//		}

	//		AudioAPI::SetMute(sound, audio.m_mute);
	//		AudioAPI::SetVolume(sound, audio.m_volume);
	//		AudioAPI::SetPitch(sound, audio.m_pitch);
	//	}
	//	
	//	// access entities with specific component
	//	//for (auto&& [entity,audio_source] : entt.GetView<AudioSource>().each())
	//	//{
	//	//	std::cout << audio_source.m_source_id << std::endl;
	//	//}

	//}

	void AudioSystem(EnttXSol& entt, EngineResources& engineResources, std::tuple<EntityData&, AudioListener&> components)
	{
		(entt); (components);

		// NOTE: ONLY ONE AUDIO LISTENER ALLOWED

		// engine resources access
		SoundManager& sound_manager = engineResources.Get<SoundManager>().get();

		// access entities with specific component
		for (auto&& [entity, audio_source] : entt.GetView<AudioSource>().each())
		{
			if (audio_source.m_sound_id != -1)
			{
				if (!audio_source.m_3d_sound)
				{
					auto& resource = sound_manager.Get(audio_source.m_sound_id);
					Sound& sound = std::get<1>(resource);

					AudioAPI::SetLoop(sound, audio_source.m_loop);

					if (audio_source.m_play_on_awake && !audio_source.m_played) // modify a bit to check for m_play_on_awake
					{
						AudioAPI::PlaySound(sound, audio_source.m_played);
					}

					AudioAPI::SetMute(sound, audio_source.m_mute);
					AudioAPI::SetVolume(sound, audio_source.m_volume);
					AudioAPI::SetPitch(sound, audio_source.m_pitch);
				}
				else
				{
					// 3d sound
					// get transform component, fmod is left handed system (need to set listener attributes)
				}
			}
		}
	}
}