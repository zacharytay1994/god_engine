#pragma once

#include "../EngineComponents/EC_All.h"
#include "../../Audio/AudioAPI.h"

bool playing = false;

namespace god
{
	void AudioSystem(EnttXSol& entt, EngineResources& engineResources, std::tuple<EntityData&, AudioListener&> components)
	{
		(entt);

		// NOTE: ONLY ONE AUDIO LISTENER ALLOWED
		AudioListener& audio_listener = std::get<1>(components);

		// engine resources access
		SoundManager& sound_manager = engineResources.Get<SoundManager>().get();
		Camera& camera = engineResources.Get<Camera>().get();

		// access entities with specific component
		for (auto&& [entity, audio_source, transform] : entt.GetView<AudioSource, Transform>().each())
		{
			if (audio_source.m_sound_id != -1)
			{
				if (!audio_source.m_3d_sound)
				{
					auto& resource = sound_manager.Get(audio_source.m_sound_id);
					Sound& sound = std::get<1>(resource);

					if (audio_source.m_reverb == nullptr)
					{
						AudioAPI::Create3DReverb(&audio_source.m_reverb);
					}

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
					AudioAPI::GLMVectorToFMODVector(transform.m_position, audio_listener.m_position);
					AudioAPI::GLMVectorToFMODVector(camera.m_look_at, audio_listener.m_forward);
					AudioAPI::GLMVectorToFMODVector(camera.m_up, audio_listener.m_up);

					// fmod is left handed system, convert to right handed system
					audio_listener.m_position.z = -audio_listener.m_position.z;
					audio_listener.m_forward.z = -audio_listener.m_forward.z;
					audio_listener.m_up.z = -audio_listener.m_up.z;
					audio_listener.m_velocity = FMOD_VECTOR(0.f, 0.f, 0.f);

					// set FMOD listener attributes
					AudioAPI::SetListenerAttributes(&audio_listener.m_position, &audio_listener.m_velocity, &audio_listener.m_forward, &audio_listener.m_up);
				}
			}
		}
	}
}