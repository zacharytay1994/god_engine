#pragma once

#include "../EngineComponents/EC_All.h"
#include "../../Audio/AudioAPI.h"

namespace god
{
	void AudioListenerSystem(EnttXSol& entt, EngineResources& engineResources, std::tuple<EntityData&, AudioListener, Transform&> components)
	{
		// NOTE: ONLY ONE AUDIO LISTENER ALLOWED
		AudioListener& audio_listener = std::get<1>(components);
		Transform& al_transform = std::get<2>(components);

		// engine resources access
		SoundManager& sound_manager = engineResources.Get<SoundManager>().get();

		// access entities with specific component
		for (auto&& [entity, active, audio_source, as_transform] : entt.GetView<AudioSource, Transform>().each())
		{
			if (audio_source.m_sound_id != -1)
			{
				auto& resource = sound_manager.Get(audio_source.m_sound_id);
				Sound& sound = std::get<1>(resource);

				AudioAPI::SetLoop(sound, audio_source.m_loop);

				if (audio_source.m_play_on_awake && !audio_source.m_awake_played)
				{
					AudioAPI::PlaySound(sound, &audio_source.m_channel, audio_source.m_awake_played);
				}
				else
				{
					if (!audio_source.m_played)
						AudioAPI::PlaySound(sound, &audio_source.m_channel, audio_source.m_played);
				}

				//if (audio_source.m_stop)
				//{
				//	AudioAPI::StopSound(audio_source.m_channel);
				//	audio_source.m_played = false;
				//	audio_source.enable_fade_in = true;
				//	audio_source.m_stop = false;
				//}

				// Fading Control --------------------------------------------------------------------
				if (audio_source.enable_fade_in)
				{
					if (audio_source.m_channel == nullptr)
						audio_source.m_played = false;

					unsigned int points;
					AudioAPI::GetFadePoints(audio_source.m_channel, &points);
					if (points == 0 && !audio_source.m_faded_in)
					{
						AudioAPI::AddFadeIn(audio_source.m_channel, audio_source.m_fade_in_time, audio_source.m_faded_in);
						audio_source.m_faded_in = true;
						audio_source.enable_fade_in = false;
					}
					else
					{
						AudioAPI::RemoveFadeIn(audio_source.m_channel, audio_source.m_fade_in_time);
						audio_source.m_faded_in = false;
					}
				}
				if (audio_source.enable_fade_out)
				{
					unsigned int points;
					AudioAPI::GetFadePoints(audio_source.m_channel, &points);
					if (points == 0 && !audio_source.m_faded_out)
					{
						AudioAPI::AddFadeOut(audio_source.m_channel, audio_source.m_fade_out_time, audio_source.m_faded_out);
						audio_source.m_faded_out = true;
						audio_source.enable_fade_out = false;
					}
					else
					{
						AudioAPI::RemoveFadeOut(audio_source.m_channel, audio_source.m_fade_out_time);
						audio_source.m_faded_out = false;
					}
				}
				// ------------------------------------------------------------------------------------

			}
		}

		AudioAPI::ToggleDSPEffects(audio_listener.dsp_effects_on);

		// 3d sound
		AudioAPI::GLMVectorToFMODVector(al_transform.m_position, audio_listener.m_position);
		AudioAPI::GLMVectorToFMODVector(glm::vec3(0.f, 0.f, 1.f), audio_listener.m_forward);
		AudioAPI::GLMVectorToFMODVector(glm::vec3(0.f, 1.f, 0.f), audio_listener.m_up);

		// set FMOD listener attributes
		AudioAPI::SetListenerAttributes(&audio_listener.m_position, nullptr, &audio_listener.m_forward, &audio_listener.m_up);

	}
}